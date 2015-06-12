#include "Chip8_InitError.h"
#include <SDL2/SDL.h>

Chip8_InitError::Chip8_InitError() noexcept {
  //ctor
}

Chip8_InitError::~Chip8_InitError() noexcept {
  //dtor
}

const char* Chip8_InitError::what() const noexcept{
  return SDL_GetError(); }

Chip8_InitError::Chip8_InitError(const Chip8_InitError& other) noexcept {
  //copy ctor
}
