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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Chip8_Typedefs.h"

class Instruction
{
    public:
        Instruction(u8 *opCode);
        Instruction(const Instruction& other);
        Instruction& operator=(const Instruction& other);
        u16 opCode() const;
        u16 NNN() const;
        u8 low() const;
        u8 high() const;
        u8 code() const;
        u8 X() const;
        u8 Y() const;
        u8 K() const;

    private:
        u16 m_opCode;
};

#endif // INSTRUCTION_H
