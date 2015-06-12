#ifndef CHIP8_INITERROR_H
#define CHIP8_INITERROR_H

#include "Chip8_Exception.h"
#include <string>


class Chip8_InitError : public Chip8_Exception
{
  public:
    Chip8_InitError() noexcept;
    virtual ~Chip8_InitError() noexcept;
    Chip8_InitError(const Chip8_InitError& other) noexcept;
    virtual const char* what() const noexcept;
  protected:
};

#endif // CHIP8_INITERROR_H
