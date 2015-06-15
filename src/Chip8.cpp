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

#include <SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <boost/log/trivial.hpp>

#include "Chip8.h"
#include "errors.h"

using namespace std;
using namespace boost;

Chip8::Chip8(Chip8Config* cfg)
try :
    m_mode(Normal),
    m_pixelSize(cfg->pixelSize()),
    m_chip8CpuFreq(cfg->cpuFreq()),
    m_romPath(cfg->romName()),
    m_sdlWindow(),
    m_sdlRenderer(),
    m_randGen(std::chrono::system_clock::now().time_since_epoch().count()),
    _frameBuffer(),
    _keyboardState(),
    _callStack(),
    _memory(),
    _V(),
    _SP(0),
    _DT(0),
    _ST(0),
    _PC(0x200),
    _I(0),
    m_keepThreadsAlive(true),
    m_DTMutex(),
    m_keyStateMutex(),
    m_cpuThread(),
    m_clockThread() {
    // Initialize SDL
    if(SDL_CreateWindowAndRenderer(64 * m_pixelSize, 32 * m_pixelSize, SDL_WINDOW_HIDDEN, &m_sdlWindow, &m_sdlRenderer)) {
        throw Chip8InitError("Cannot initialize SDL");
    }

    BOOST_LOG_TRIVIAL(info) << "SDL initialized successfully";

    // Initialize stack and register arrays
    memset(_callStack, 0, 16*sizeof(_callStack[0]));
    memset(_callStack, 0, 16*sizeof(_V[0]));

    // Initialize Sprite Data
    _memory[0] = 240;  _memory[1] = 144;  _memory[2] = 144;  _memory[3] = 144;
    _memory[4] = 240;  _memory[5] = 32;   _memory[6] = 96;   _memory[7] = 32;
    _memory[8] = 32;   _memory[9] = 112;  _memory[10] = 240; _memory[11] = 16;
    _memory[12] = 240; _memory[13] = 128; _memory[14] = 240; _memory[15] = 240;
    _memory[16] = 16;  _memory[17] = 240; _memory[18] = 16;  _memory[19] = 240;
    _memory[20] = 144; _memory[21] = 144; _memory[22] = 240; _memory[23] = 16;
    _memory[24] = 16;  _memory[25] = 240; _memory[26] = 128; _memory[27] = 240;
    _memory[28] = 16;  _memory[29] = 240; _memory[30] = 240; _memory[31] = 128;
    _memory[32] = 240; _memory[33] = 144; _memory[34] = 240; _memory[35] = 240;
    _memory[36] = 16;  _memory[37] = 32;  _memory[38] = 64;  _memory[39] = 64;
    _memory[40] = 240; _memory[41] = 144; _memory[42] = 240; _memory[43] = 144;
    _memory[44] = 240; _memory[45] = 240; _memory[46] = 144; _memory[47] = 240;
    _memory[48] = 16;  _memory[49] = 240; _memory[50] = 240; _memory[51] = 144;
    _memory[52] = 240; _memory[53] = 144; _memory[54] = 144; _memory[55] = 224;
    _memory[56] = 144; _memory[57] = 224; _memory[58] = 144; _memory[59] = 224;
    _memory[60] = 240; _memory[61] = 128; _memory[62] = 128; _memory[63] = 128;
    _memory[64] = 240; _memory[65] = 224; _memory[66] = 144; _memory[67] = 144;
    _memory[68] = 144; _memory[69] = 224; _memory[70] = 240; _memory[71] = 128;
    _memory[72] = 240; _memory[73] = 128; _memory[74] = 240; _memory[75] = 240;
    _memory[76] = 128; _memory[77] = 240; _memory[78] = 128; _memory[79] = 128;

    // Filling the rest with 0's
    memset(&(_memory[80]), 0, (4096-80)*sizeof(_memory[0]));
} catch(...) {
    throw;
}

Chip8::~Chip8() noexcept {
    // Kill active threads
    m_keepThreadsAlive = false;
    m_clockThread.join();
    m_cpuThread.join();

    // Destroy SDL context and we should be good to go
    SDL_DestroyRenderer(m_sdlRenderer);
    SDL_DestroyWindow(m_sdlWindow);
}

int Chip8::loadRom() {
    ifstream romFile(m_romPath, ios_base::in | ios_base::binary | ios_base::ate);
    if(!(romFile.is_open())) {
        throw Chip8FileError(m_romPath);
    }

    // Get rom size and load it at memory address 0x0200
    size_t romSize = romFile.tellg();
    romFile.seekg(0);
    romFile.read(reinterpret_cast<char*>(&(_memory[0x0200])), romSize);
    romFile.close();

    BOOST_LOG_TRIVIAL(info) << "Rom " << m_romPath << " successfully loaded!";

    return 0;
}

void Chip8::run() {
    // Load the ROM
    try {
        BOOST_LOG_TRIVIAL(error) << "Loading ROM " << m_romPath;
        loadRom();
        m_cpuThread = thread(&Chip8::_CPUThread, this);
        m_clockThread = thread(&Chip8::_ClockThread, this);
    } catch(Chip8FileError const& e) {
        BOOST_LOG_TRIVIAL(error) << "Error loading rom " << e.what();
        throw;
    } catch(system_error const& e) {
        BOOST_LOG_TRIVIAL(fatal) << "Couldn't create thread.";
        throw;
    } catch(...) {
        BOOST_LOG_TRIVIAL(fatal) << "Unexpected exception when trying to load the ROM.";
        throw;
    }

    bool end = false;
    SDL_Event event;

    // TODO : is this really necessary? There isn't a GUI anyway, no point in hiding the window.
    // We can now show the SDL window, and hide it when the loop ends.
    SDL_ShowWindow(m_sdlWindow);

    while(!end) {
        SDL_WaitEvent(&event);
        if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) { // If a key was pressed or released...
            bool pressed = (event.type == SDL_KEYDOWN); // find out which one it was...
            m_keyStateMutex.lock();
            switch(event.key.keysym.sym) { // and update keystate
            /*
            TODO
            Not pretty. There's gotta be a better, less dumb way.
            */
            case SDLK_KP_0:
                _keyboardState.set(0, pressed);
                break;
            case SDLK_KP_1:
                _keyboardState.set(7, pressed);
                break;
            case SDLK_KP_2:
                _keyboardState.set(8, pressed);
                break;
            case SDLK_KP_3:
                _keyboardState.set(9, pressed);
                break;
            case SDLK_KP_4:
                _keyboardState.set(4, pressed);
                break;
            case SDLK_KP_5:
                _keyboardState.set(5, pressed);
                break;
            case SDLK_KP_6:
                _keyboardState.set(6, pressed);
                break;
            case SDLK_KP_7:
                _keyboardState.set(1, pressed);
                break;
            case SDLK_KP_8:
                _keyboardState.set(2, pressed);
                break;
            case SDLK_KP_9:
                _keyboardState.set(3, pressed);
                break;
            case SDLK_a:
                _keyboardState.set(0xA, pressed);
                break;
            case SDLK_b:
                _keyboardState.set(0xB, pressed);
                break;
            case SDLK_c:
                _keyboardState.set(0xC, pressed);
                break;
            case SDLK_d:
                _keyboardState.set(0xD, pressed);
                break;
            case SDLK_e:
                _keyboardState.set(0xE, pressed);
                break;
            case SDLK_f:
                _keyboardState.set(0xF, pressed);
                break;
            default:
                break;
            }
            m_keyStateMutex.unlock();
        } else if(event.type == SDL_QUIT)
            end = true;
    }
    SDL_HideWindow(m_sdlWindow);
}

bool Chip8::xorPixel(int x, int y, bool val) {
    // Wrap around
    if(x >= 64 || y >= 32) {
        x %= 64;
        y %= 32;
    }

    bool curState = _frameBuffer[x + 64 * y];
    _frameBuffer.set(x + 64 * y, curState ^ val);

    // Collision detection
    if(curState && val) {
        return true;
    } else {
        return false;
    }
}

void Chip8::_ClockThread() {
    // TODO : shouldn't this value depend from the configured CPU clock? I'm pretty sure it should
    // Well, after some thinking, it isn't. This value is what actually speeds up/slows down the
    // emulation, not really the CPU clock. I should experiment with the values to make sure.
    std::chrono::microseconds step(16666);
    while(m_keepThreadsAlive) {
        if(_DT || _ST) {
            m_DTMutex.lock();
            if(_DT) {
                --_DT;
            }
            if(_ST) {
                --_ST;
            }
            m_DTMutex.unlock();
        }
        std::this_thread::sleep_for(step);
    }
}

void Chip8::_CPUThread() {
    byte lowByte, hiByte, code, X, Y, K;
    unsigned short NNN;

    // Compute clock cycle time
    std::chrono::microseconds cycleDuration(1000000 / m_chip8CpuFreq);

    while(m_keepThreadsAlive) {
        std::chrono::high_resolution_clock::time_point instrStartTime = std::chrono::high_resolution_clock::now();
        lowByte = _memory[_PC + 1];
        hiByte = _memory[_PC];
        code = (hiByte & 0xF0) / 0x10;
        X = hiByte & 0x0F;
        Y = (lowByte & 0xF0) / 0x10;
        K = lowByte & 0x0F;
        NNN = (X * 0x100) + lowByte;

        try {
            /*
            TODO
            There might be a better way than this. Think about it. 100loc long switch? Ugly.
            */
            switch(code) {
            case 0:
                if(NNN == 0x0E0) {
                    _instr00E0();
                    _PC+=2;
                    break;
                }
                else if(NNN == 0x0EE) {
                    _instr00EE();
                    break;
                }
                else if(NNN == 0x230) {
                    _instr0230();
                }
                else { // opcode 0nnn, ignored
                    break;
                }
            case 1:
                _instr1NNN(NNN);
                break;
            case 2:
                _instr2NNN(NNN);
                break;
            case 3:
                _instr3XKK(X, lowByte);
                break;
            case 4:
                _instr4XKK(X, lowByte);
                break;
            case 5:
                if(!K)
                    _instr5XY0(X, Y);
                break;
            case 6:
                _instr6XKK(X, lowByte);
                break;
            case 7:
                _instr7XKK(X, lowByte);
                break;
            case 8:
                switch(K) {
                case 0:
                    _instr8XY0(X, Y);
                    break;
                case 1:
                    _instr8XY1(X, Y);
                    break;
                case 2:
                    _instr8XY2(X, Y);
                    break;
                case 3:
                    _instr8XY3(X, Y);
                    break;
                case 4:
                    _instr8XY4(X, Y);
                    break;
                case 5:
                    _instr8XY5(X, Y);
                    break;
                case 6:
                    _instr8XY6(X);
                    break;
                case 7:
                    _instr8XY7(X, Y);
                    break;
                case 0xE:
                    _instr8XYE(X);
                    break;
                }
            case 9:
                if(!K)
                    _instr9XY0(X, Y);
                break;
            case 0xA:
                _instrANNN(NNN);
                break;
            case 0xB:
                _instrBNNN(NNN);
                break;
            case 0xC:
                _instrCXKK(X, lowByte);
                break;
            case 0xD:
                _instrDXYK(X, Y, K);
                break;
            case 0xE:
                if(lowByte == 0x9E) {
                    _instrEX9E(X);
                } else if(lowByte == 0xA1) {
                    _instrEXA1(X);
                }
                break;
            case 0xF:
                switch(lowByte) {
                case 0x07:
                    _instrFX07(X);
                    break;
                case 0x0A:
                    _instrFX0A(X);
                    break;
                case 0x15:
                    _instrFX15(X);
                    break;
                case 0x18:
                    _instrFX18(X);
                    break;
                case 0x1E:
                    _instrFX1E(X);
                    break;
                case 0x29:
                    _instrFX29(X);
                    break;
                case 0x33:
                    _instrFX33(X);
                    break;
                case 0x55:
                    _instrFX55(X);
                    break;
                case 0x65:
                    _instrFX65(X);
                    break;
                }
                break;
            default:
                throw Chip8UnknownOpcodeError(hiByte * 0x100 + lowByte);
            }
            if(code != 0xB && code != 0x1 && code != 0x2)
                _PC+=2;
            this_thread::sleep_until(instrStartTime + cycleDuration);
        } catch(Chip8UnknownOpcodeError e) {
            cerr << "Warning : unknown opcode. Trying to continue..." << endl;
            BOOST_LOG_TRIVIAL(error) << e.what();
        } catch(...) {
            cerr << "Unknown exception on CPU thread. Aborting.\nSee log file for details." << endl;
            BOOST_LOG_TRIVIAL(fatal) << "Unknown exception in CPU thread.";
        }
    }
}
