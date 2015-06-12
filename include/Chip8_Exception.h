#ifndef CHIP8_EXCEPTION_H
#define CHIP8_EXCEPTION_H

#include <exception>

class Chip8_Exception: public std::exception
{
  public:
    Chip8_Exception() noexcept;
    Chip8_Exception(Chip8_Exception const& other) noexcept;
    virtual const char* what() const noexcept = 0;
    virtual ~Chip8_Exception() noexcept;
};

#endif // CHIP8_EXCEPTION_H
