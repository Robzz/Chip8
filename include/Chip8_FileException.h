#ifndef CHIP8_FILEEXCEPTION_H
#define CHIP8_FILEEXCEPTION_H

#include "Chip8_Exception.h"
#include <string>

class Chip8_FileException : public Chip8_Exception
{
  public:
    Chip8_FileException(std::string const& fileName = "") noexcept;
    Chip8_FileException(Chip8_FileException const& other) noexcept;
    virtual const char* what() const noexcept;
    virtual ~Chip8_FileException() noexcept;

  protected:
    std::string m_FileName;
};

#endif // CHIP8_FILEEXCEPTION_H
