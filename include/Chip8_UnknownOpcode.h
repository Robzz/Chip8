#ifndef CHIP8_UNKNOWNOPCODE_H
#define CHIP8_UNKNOWNOPCODE_H

#include "Chip8_Exception.h"


class Chip8_UnknownOpcode : public Chip8_Exception
{
  public:
    Chip8_UnknownOpcode(unsigned short opCode = 0) noexcept;
    virtual ~Chip8_UnknownOpcode() noexcept;
    Chip8_UnknownOpcode(const Chip8_UnknownOpcode& other) noexcept;
    virtual const char* what() const noexcept;
    short m_opCode;
};

#endif // CHIP8_UNKNOWNOPCODE_H
