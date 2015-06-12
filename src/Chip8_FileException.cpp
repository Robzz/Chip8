#include "Chip8_FileException.h"
#include <sstream>

using namespace std;

Chip8_FileException::Chip8_FileException(std::string const& fileName) noexcept :
  Chip8_Exception(),
  m_FileName(fileName)
{ }

Chip8_FileException::Chip8_FileException(Chip8_FileException const& other) noexcept :
  Chip8_Exception(),
  m_FileName(other.m_FileName)
{
  //ctor
}

const char* Chip8_FileException::what() const noexcept {
  ostringstream ss;
  ss << "Chip8_FileException : " << m_FileName << endl;
  return ss.str().c_str(); }

Chip8_FileException::~Chip8_FileException() noexcept
{ }
