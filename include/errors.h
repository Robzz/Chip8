#ifndef CHIP8_ERRORS_H
#define CHIP8_ERRORS_H

#include <exception>
#include <string>

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
    Chip8InitError() noexcept;
    virtual ~Chip8InitError() noexcept;
    Chip8InitError(const Chip8InitError& other) noexcept;
    virtual const char* what() const noexcept;
  protected:
};

class Chip8UnknownOpcodeError : public Chip8Exception
{
  public:
    Chip8UnknownOpcodeError(unsigned short opCode = 0) noexcept;
    virtual ~Chip8UnknownOpcodeError() noexcept;
    Chip8UnknownOpcodeError(const Chip8UnknownOpcodeError& other) noexcept;
    virtual const char* what() const noexcept;
    short m_opCode;
};

#endif
