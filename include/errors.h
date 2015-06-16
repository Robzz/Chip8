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

#ifndef CHIP8_ERRORS_H
#define CHIP8_ERRORS_H

#include <exception>
#include <string>

#include "Chip8_Typedefs.h"

class Chip8Exception: public std::exception
{
  public:
    Chip8Exception() noexcept;
    Chip8Exception(Chip8Exception const& other) noexcept;
    virtual const char* what() const noexcept = 0;
    virtual ~Chip8Exception() noexcept;
};

class Chip8FileError : public Chip8Exception
{
  public:
    Chip8FileError(std::string const& fileName = "") noexcept;
    Chip8FileError(Chip8FileError const& other) noexcept;
    virtual const char* what() const noexcept;
    virtual ~Chip8FileError() noexcept;

  protected:
    std::string m_FileName;
};

class Chip8InitError : public Chip8Exception
{
  public:
    Chip8InitError(std::string const& message) noexcept;
    virtual ~Chip8InitError() noexcept;
    Chip8InitError(const Chip8InitError& other) noexcept;
    virtual const char* what() const noexcept;
  protected:
      std::string m_message;
};

class Chip8UnknownOpcodeError : public Chip8Exception
{
  public:
    Chip8UnknownOpcodeError(u16 opCode = 0) noexcept;
    virtual ~Chip8UnknownOpcodeError() noexcept;
    Chip8UnknownOpcodeError(const Chip8UnknownOpcodeError& other) noexcept;
    virtual const char* what() const noexcept;
    short m_opCode;
};

#endif
