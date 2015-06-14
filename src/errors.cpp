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

#include "errors.h"
#include <sstream>
#include <SDL.h>

using namespace std;

Chip8Exception::Chip8Exception() noexcept { }

Chip8Exception::Chip8Exception(Chip8Exception const& other) noexcept { }

Chip8Exception::~Chip8Exception() noexcept { }

Chip8FileError::Chip8FileError(std::string const& fileName) noexcept :
    Chip8Exception(),
    m_FileName(fileName)
{ }


Chip8FileError::Chip8FileError(Chip8FileError const& other) noexcept :
    Chip8Exception(),
    m_FileName(other.m_FileName)
{ }

const char* Chip8FileError::what() const noexcept {
    ostringstream ss;
    ss << "Chip8FileError : " << m_FileName << endl;
    return ss.str().c_str();
}

Chip8FileError::~Chip8FileError() noexcept { }


Chip8InitError::Chip8InitError(string const& message) noexcept :
    m_message(message)
{ }

Chip8InitError::~Chip8InitError() noexcept { }

Chip8InitError::Chip8InitError(const Chip8InitError& other) noexcept :
    m_message(other.m_message) { }

const char* Chip8InitError::what() const noexcept {
    return m_message.c_str();
}


Chip8UnknownOpcodeError::Chip8UnknownOpcodeError(unsigned short opCode) noexcept :
    m_opCode(opCode)
{ }

Chip8UnknownOpcodeError::~Chip8UnknownOpcodeError() noexcept { }

Chip8UnknownOpcodeError::Chip8UnknownOpcodeError(const Chip8UnknownOpcodeError& other) noexcept :
    m_opCode(other.m_opCode)
{ }

const char* Chip8UnknownOpcodeError::what() const noexcept {
    ostringstream ss;
    ss << "Chip8UnknownOpcodeError : 0x" << m_opCode << endl;
    return ss.str().c_str();
}
