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


Chip8InitError::Chip8InitError() noexcept { }

Chip8InitError::~Chip8InitError() noexcept { }

const char* Chip8InitError::what() const noexcept{
  return SDL_GetError(); }

Chip8InitError::Chip8InitError(const Chip8InitError& other) noexcept { }


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
