/*  Chip8++ - A CHIP-8 emulator.
    Copyright (C) 2015  Robin Chavignat

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "Chip8.h"
#include <SDL.h>
#include <boost/log/trivial.hpp>

/* Clear the screen with the background color. */
void Chip8::_instr00E0() {
    SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(m_sdlRenderer);
    SDL_RenderPresent(m_sdlRenderer);
}

/* Return from a subroutine. */
void Chip8::_instr00EE() {
    _PC = _callStack[_SP];
    --_SP;
}
/* CLS (Clear screen in HiRes mode) */
void Chip8::_instr0230() {
    _instr00E0();
}

/* Jump to the address pointed by NNN.
   Switch to Hi-Res mode and jump to address 0x2C0 if it is the first instruction and the jump
   address is 0x260 */
void Chip8::_instr1NNN(unsigned short NNN) {
    if(_PC == 0x200 && NNN == 0x260 && m_mode == Normal) {
        BOOST_LOG_TRIVIAL(info) << "Entering HiRes mode.";
        SDL_SetWindowSize(m_sdlWindow, 64*m_pixelSize, 64*m_pixelSize);
        m_mode = HiRes;
        _PC = 0x2C0;
    }
    else {
        _PC = NNN;
    }
}

/* Call subroutine at NNN. */
void Chip8::_instr2NNN(unsigned short NNN) {
    ++_SP;
    _callStack[_SP] = _PC;
    _PC = NNN;
}

/* Skips next instruction if the register Vx is equal to KK. */
void Chip8::_instr3XKK(byte X, byte KK) {
    if(_V[X] == KK)
        _PC += 2;
}

/* Skips next instruction if the register Vx is not equal to KK. */
void Chip8::_instr4XKK(byte X, byte KK) {
    if(_V[X] != KK)
        _PC +=2;
}

/* Skips next instruction if the register Vx is equal to Vy. */
void Chip8::_instr5XY0(byte X, byte Y) {
    if(_V[X] == _V[Y])
        _PC += 2;
}

/* Load the value KK into the register Vx. */
void Chip8::_instr6XKK(byte X, byte KK) {
    _V[X] = KK;
}

/* Add KK to the value in Vx, and store the result in Vx */
void Chip8::_instr7XKK(byte X, byte KK) {
    _V[X] += KK;
}

/* Store the value of the register Vy in the register Vx. */
void Chip8::_instr8XY0(byte X, byte Y) {
    _V[X] = _V[Y];
}

/* Performs a bitwise OR between register Vx and Vy, and stores the result in Vx. */
void Chip8::_instr8XY1(byte X, byte Y) {
    _V[X] |= _V[Y];
}

/* Performs a bitwise AND between register Vx and Vy, and stores the result in Vx. */
void Chip8::_instr8XY2(byte X, byte Y) {
    _V[X] &= _V[Y];
}

/* Performs a bitwise XOR between register Vx and Vy, and stores the result in Vx. */
void Chip8::_instr8XY3(byte X, byte Y) {
    _V[X] ^= _V[Y];
}

/* Performs the addition of Vx and Vy, stores the result in Vx and the carry in VF. */
void Chip8::_instr8XY4(byte X, byte Y) {
    unsigned short bigger = _V[X] + _V[Y];
    _V[X] += _V[Y];
    if(bigger & 0xFF00)
        _V[0xF] = 1;
    else _V[0xF] = 0;
}

/* Performs the substrasction of Vx and Vy, and stores the result in Vx.
   Set VF to 1 if Vx > Vy, to 0 otherwise. */
void Chip8::_instr8XY5(byte X, byte Y) {
    if(_V[X] > _V[Y])
        _V[0xF] = 1;
    else _V[0xF] = 0;
    _V[X] -= _V[Y];
}

/* If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2. */
void Chip8::_instr8XY6(byte X) {
    _V[0xF] = 1 & _V[X];
    _V[X] /= 2;
}

/* Performs the substrasction of Vy and Vx, and stores the result in Vx.
   Set VF to 1 if Vy > Vx, to 0 otherwise. */
void Chip8::_instr8XY7(byte X, byte Y) {
    if(_V[X] < _V[Y])
        _V[0xF] = 1;
    else _V[0xF] = 0;
    _V[X] = _V[Y] - _V[X];
}

/* If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2. */
void Chip8::_instr8XYE(byte X) {
    _V[0xF] = _V[X] & 0x80;
    _V[X] *= 2;
}

/* Skip next instruction if Vx != Vy. */
void Chip8::_instr9XY0(byte X, byte Y) {
    if(_V[X] != _V[Y])
        _PC += 2;
}

/* Set the value of the register I to NNN. */
void  Chip8::_instrANNN(unsigned short NNN) {
    _I = NNN;
}

/* Jump to address NNN + V0. */
void Chip8::_instrBNNN(unsigned short NNN) {
    _PC = NNN + _V[0];
}

/* Generate a random byte and AND it with KK. The result is stored in Vx. */
void Chip8::_instrCXKK(byte X, byte KK) {
    byte r = m_randGen() % 255;
    _V[X] = r & KK;
}

/* Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. */
void Chip8::_instrDXYK(byte X, byte Y, byte K) {
    // XOR Sprite over framebuffer
    bool collision = false;
    for(int i = 0 ; i != K ; ++i) {
        int xOffsetCounter = 0;
        for(int it = 0x80 ; it != 0 ; it /= 2) {
            if(xorPixel(_V[X] + xOffsetCounter, _V[Y] + i, _memory[_I + i] & it))
                collision = true;
            ++xOffsetCounter;
        }
    }

    // And then, render
    std::vector<SDL_Rect> pixel_rects;
    int nPixels = (m_mode == Normal) ? 32*64 : 64*64;
    for(int i = 0 ; i != nPixels ; ++i) {
        if(_frameBuffer[i]) {
            SDL_Rect pix;
            pix.x = (i % 64) * m_pixelSize;
            pix.y = i / 64 * m_pixelSize;
            pix.h = m_pixelSize;
            pix.w = m_pixelSize;
            pixel_rects.push_back(pix);
        }
    }
    if(pixel_rects.size()) {
        SDL_SetRenderDrawColor(m_sdlRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(m_sdlRenderer);
        SDL_SetRenderDrawColor(m_sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRects(m_sdlRenderer, &pixel_rects[0], pixel_rects.size());
        SDL_RenderPresent(m_sdlRenderer);
    }
    _V[0xF] = (collision) ? 1 : 0;
}

/* Skip next instruction if key with the value of Vx is pressed. */
void Chip8::_instrEX9E(byte X) {
    m_keyStateMutex.lock();
    if(_keyboardState[_V[X]])
        _PC+=2;
    m_keyStateMutex.unlock();
}

/* Skip next instruction if key with the value of Vx is not pressed. */
void Chip8::_instrEXA1(byte X) {
    m_keyStateMutex.lock();
    if(!_keyboardState[_V[X]])
        _PC+=2;
    m_keyStateMutex.unlock();
}

/* Load the value of the DT register into Vx. */
void Chip8::_instrFX07(byte X) {
    m_DTMutex.lock();
    _V[X] = _DT;
    m_DTMutex.unlock();
}

/* Wait for a key press, store the value of the key in Vx. */
void Chip8::_instrFX0A(byte X) {
    m_keyStateMutex.lock();
    // Find a way to pause execution til a key is pressed...
    m_keyStateMutex.unlock();
}

/* Set the DT register to the value of Vx. */
void Chip8::_instrFX15(byte X) {
    m_DTMutex.lock();
    _DT = _V[X];
    m_DTMutex.unlock();
}

/* Set the ST register to the value of Vx. */
void Chip8::_instrFX18(byte X) {
    m_DTMutex.lock();
    _ST = _V[X];
    m_DTMutex.unlock();
}

/* Add the value of Vx to I, and store the result in I. */
void Chip8::_instrFX1E(byte X) {
    _I += _V[X];
}

/* Set I to the location of the sprite for the digit stored in Vx. */
void Chip8::_instrFX29(byte X) {
    _I = _V[X] * 5;
}

/* Store BCD representation of Vx in memory locations I, I+1, and I+2. */
void Chip8::_instrFX33(byte X) {
    _memory[_I] = _V[X] / 100;
    _memory[_I] = (_V[X] % 100) / 10;
    _memory[_I] = _V[X] % 10;
}

/* Store registers V0 through Vx in memory starting at location I. */
void Chip8::_instrFX55(byte X) {
    for(int i = 0 ; i != 0x10; ++i)
        _memory[_I + i] = _V[i];
}

/* Read registers V0 through Vx from memory starting at location I. */
void Chip8::_instrFX65(byte X) {
    for(int i = 0 ; i != 0x10; ++i)
        _V[i] = _memory[_I + i];
}
