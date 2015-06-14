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

#ifndef CHIP8_H
#define CHIP8_H

#include <random>
#include <bitset>
#include <thread>
#include <mutex>

#include "Chip8_Typedefs.h"
#include "Chip8Config.h"
#include "Dispatcher.h"

// Forward declarations
struct SDL_Window;
struct SDL_Renderer;

class Chip8 {
  public:
    enum Chip8_Keydown { Key_0 = 0x1, Key_1 = 0x2, Key_2 = 0x4, Key_3 = 0x8,
            Key_4 = 0x10, Key_5 = 0x20, Key_6 = 0x40, Key_7 = 0x80,
            Key_8 = 0x100, Key_9 = 0x200, Key_A = 0x400, Key_B = 0x800,
            Key_C = 0x1000, Key_D = 0x2000, Key_E = 0x4000, Key_F = 0x8000
                       };
    Chip8(Chip8Config* cfg);
    // Starts the emulation, the main loop takes place here
    void run();
    virtual ~Chip8() noexcept;

  private:
    int m_PixelSize;
    int m_Chip8CpuFreq;
    std::string m_RomPath;

    Dispatcher m_Dispatch; // Instruction dispatcher
    SDL_Window* m_SDLWindow;
    SDL_Renderer* m_SDLRenderer;
    std::minstd_rand m_RandGen;

    std::bitset<2048> _FrameBuffer;
    std::bitset<16> _KeyboardState;
    unsigned short _CallStack[16];
    byte _Memory[4096];
    byte _V[16];
    byte _SP;
    byte _DT;
    byte _ST;
    unsigned short _PC;
    unsigned short _I;

    bool m_KeepThreadsAlive;
    std::mutex m_DTmutex;
    std::mutex m_KeyStateMutex;
    std::thread m_CPUThread;
    std::thread m_ClockThread;

    // Loads the ROM whose path is stored in m_Rompath
    int loadRom();

    // XOR's the pixel at (x,y) coordinates over framebuffer
    // Returns true in case of collision (val = framebuffer(x,y) = true)
    bool xorPixel(int x, int y, bool val);

    // Timer tick thread
    void _ClockThread();

    // CPU emu thread
    void _CPUThread();

    // Chip8 instruction set
    void _instr00E0(); // CLS (CLear Screen)
    void _instr00EE(); // RET (RETurn from subroutine)
    void _instr1NNN(unsigned short NNN); // JMP
    void _instr2NNN(unsigned short NNN); // CALL
    void _instr3XKK(byte X, byte KK); // SE (Skip Equal)
    void _instr4XKK(byte X, byte KK); // SNE (Skip Not Equal)
    void _instr5XY0(byte X, byte Y); // SE
    void _instr6XKK(byte X, byte KK); // LD (load immediate)
    void _instr7XKK(byte X, byte KK); // ADD
    void _instr8XY0(byte X, byte Y); // LD (load)
    void _instr8XY1(byte X, byte Y); // OR
    void _instr8XY2(byte X, byte Y); // AND
    void _instr8XY3(byte X, byte Y); // XOR
    void _instr8XY4(byte X, byte Y); // ADD
    void _instr8XY5(byte X, byte Y); // SUB
    void _instr8XY6(byte X); // SHR (SHift Right)
    void _instr8XY7(byte X, byte Y); // SUBN
    void _instr8XYE(byte X); // SHL (SHift Left)
    void _instr9XY0(byte X, byte Y); // SNE
    void _instrANNN(unsigned short NNN); // LD I
    void _instrBNNN(unsigned short NNN); // JMP
    void _instrCXKK(byte X, byte KK); // RND
    void _instrDXYK(byte X, byte Y, byte K); // DRW
    void _instrEX9E(byte X); // SKP (SKip Pressed)
    void _instrEXA1(byte X); // SKNP (SKip Not Pressed)
    void _instrFX07(byte X);
    void _instrFX0A(byte X);
    void _instrFX15(byte X);
    void _instrFX18(byte X);
    void _instrFX1E(byte X);
    void _instrFX29(byte X);
    void _instrFX33(byte X);
    void _instrFX55(byte X);
    void _instrFX65(byte X);

    Chip8();
};

#endif // CHIP8_H
