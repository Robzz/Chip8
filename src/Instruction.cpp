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

#include "Instruction.h"

Instruction::Instruction(u8* opCode)
{
    //ctor
}

Instruction::Instruction(const Instruction& other)
{
    //copy ctor
}

Instruction& Instruction::operator=(const Instruction& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

u16 Instruction::opCode() const {
    return m_opCode;
}


/*lowu8 = _memory[_PC + 1];
        hiu8 = _memory[_PC];
        code = (hiu8 & 0xF0) / 0x10;
        X = hiu8 & 0x0F;
        Y = (lowu8 & 0xF0) / 0x10;
        K = lowu8 & 0x0F;
        NNN = (X * 0x100) + lowu8;*/
u16 Instruction::NNN() const {
    return m_opCode;
}

u8 Instruction::low() const {
    return m_opCode & 0x00FF;
}

u8 Instruction::high() const {
    return m_opCode;
}

u8 Instruction::code() const {
    return m_opCode;
}

u8 Instruction::X() const {
    return m_opCode;
}

u8 Instruction::Y() const {
    return m_opCode;
}

u8 Instruction::K() const {
    return m_opCode;
}
