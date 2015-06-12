#include "Chip8_UnknownOpcode.h"
#include <sstream>

using namespace std;

Chip8_UnknownOpcode::Chip8_UnknownOpcode(unsigned short opCode) noexcept :
  m_opCode(opCode)
{
  //ctor
}

Chip8_UnknownOpcode::~Chip8_UnknownOpcode() noexcept
{
  //dtor
}

Chip8_UnknownOpcode::Chip8_UnknownOpcode(const Chip8_UnknownOpcode& other) noexcept :
    m_opCode(other.m_opCode)
{ }

const char* Chip8_UnknownOpcode::what() const noexcept {
  ostringstream ss;
  ss << "Chip8_UnknownOpcode : 0x" << m_opCode << endl;
  return ss.str().c_str(); }
