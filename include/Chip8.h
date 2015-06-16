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

// Forward declarations
struct SDL_Window;
struct SDL_Renderer;

class Chip8 {
  public:
    enum Chip8_Keydown {Key_0 = 0x1,    Key_1 = 0x2,    Key_2 = 0x4,    Key_3 = 0x8,
                        Key_4 = 0x10,   Key_5 = 0x20,   Key_6 = 0x40,   Key_7 = 0x80,
                        Key_8 = 0x100,  Key_9 = 0x200,  Key_A = 0x400,  Key_B = 0x800,
                        Key_C = 0x1000, Key_D = 0x2000, Key_E = 0x4000, Key_F = 0x8000};

    enum Chip8_Mode {Normal, HiRes, Super, Mega};
    Chip8(Chip8Config* cfg);
    // Starts the emulation, the main loop takes place here
    void run();
    virtual ~Chip8() noexcept;

  private:
    Chip8_Mode m_mode;
    int m_pixelSize;
    int m_chip8CpuFreq;
    std::string m_romPath;

    SDL_Window* m_sdlWindow;
    SDL_Renderer* m_sdlRenderer;
    std::minstd_rand m_randGen;

    std::bitset<4096> _frameBuffer;
    std::bitset<16> _keyboardState;
    u16 _callStack[16];
    u8 _memory[4096];
    u8 _V[16];
    u8 _SP;
    u8 _DT;
    u8 _ST;
    u16 _PC;
    u16 _I;

    bool m_keepThreadsAlive;
    std::mutex m_DTMutex;
    std::mutex m_keyStateMutex;
    std::thread m_cpuThread;
    std::thread m_clockThread;

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
    void _instr0230(); // CLS (Clear screen HiRes mode)
    void _instr1NNN(u16 NNN); // JMP
    void _instr2NNN(u16 NNN); // CALL
    void _instr3XKK(u8 X, u8 KK); // SE (Skip Equal)
    void _instr4XKK(u8 X, u8 KK); // SNE (Skip Not Equal)
    void _instr5XY0(u8 X, u8 Y); // SE
    void _instr6XKK(u8 X, u8 KK); // LD (load immediate)
    void _instr7XKK(u8 X, u8 KK); // ADD
    void _instr8XY0(u8 X, u8 Y); // LD (load)
    void _instr8XY1(u8 X, u8 Y); // OR
    void _instr8XY2(u8 X, u8 Y); // AND
    void _instr8XY3(u8 X, u8 Y); // XOR
    void _instr8XY4(u8 X, u8 Y); // ADD
    void _instr8XY5(u8 X, u8 Y); // SUB
    void _instr8XY6(u8 X); // SHR (SHift Right)
    void _instr8XY7(u8 X, u8 Y); // SUBN
    void _instr8XYE(u8 X); // SHL (SHift Left)
    void _instr9XY0(u8 X, u8 Y); // SNE
    void _instrANNN(u16 NNN); // LD I
    void _instrBNNN(u16 NNN); // JMP
    void _instrCXKK(u8 X, u8 KK); // RND
    void _instrDXYK(u8 X, u8 Y, u8 K); // DRW
    void _instrEX9E(u8 X); // SKP (SKip Pressed)
    void _instrEXA1(u8 X); // SKNP (SKip Not Pressed)
    void _instrFX07(u8 X);
    void _instrFX0A(u8 X);
    void _instrFX15(u8 X);
    void _instrFX18(u8 X);
    void _instrFX1E(u8 X);
    void _instrFX29(u8 X);
    void _instrFX33(u8 X);
    void _instrFX55(u8 X);
    void _instrFX65(u8 X);

    Chip8();
};

#endif // CHIP8_H
