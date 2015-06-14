#include "Chip8.h"
#include <SDL.h>

/*
TODO
May not be completely stupid to add a comment or two, heh?
Other than that, most of this looks okay-ish.
*/

void Chip8::_instr00E0() {
  SDL_SetRenderDrawColor(m_SDLRenderer, 0, 0, 0, 0xFF);
  SDL_RenderClear(m_SDLRenderer);
  SDL_RenderPresent(m_SDLRenderer); }

void Chip8::_instr00EE() {
  _PC = _CallStack[_SP];
  --_SP; }

void Chip8::_instr1NNN(unsigned short NNN){
  _PC = NNN; }

void Chip8::_instr2NNN(unsigned short NNN) {
  ++_SP;
  _CallStack[_SP] = _PC;
  _PC = NNN; }

void Chip8::_instr3XKK(byte X, byte KK) {
 if(_V[X] == KK)
    _PC += 2; }

void Chip8::_instr4XKK(byte X, byte KK) {
  if(_V[X] != KK)
    _PC +=2; }

void Chip8::_instr5XY0(byte X, byte Y) {
  if(_V[X] == _V[Y])
    _PC += 2; }

void Chip8::_instr6XKK(byte X, byte KK) {
  _V[X] = KK; }

void Chip8::_instr7XKK(byte X, byte KK) {
  _V[X] += KK; }

void Chip8::_instr8XY0(byte X, byte Y) {
  _V[X] = _V[Y]; }

void Chip8::_instr8XY1(byte X, byte Y) {
  _V[X] |= _V[Y]; }

void Chip8::_instr8XY2(byte X, byte Y) {
  _V[X] &= _V[Y]; }

void Chip8::_instr8XY3(byte X, byte Y) {
  _V[X] ^= _V[Y]; }

void Chip8::_instr8XY4(byte X, byte Y) {
  unsigned short bigger = _V[X] + _V[Y];
  _V[X] += _V[Y];
  if(bigger & 0xFF00)
    _V[0xF] = 1;
  else _V[0xF] = 0; }

void Chip8::_instr8XY5(byte X, byte Y) {
  if(_V[X] > _V[Y])
    _V[0xF] = 1;
  else _V[0xF] = 0;
  _V[X] -= _V[Y]; }

void Chip8::_instr8XY6(byte X) {
  _V[0xF] = 1 & _V[X];
  _V[X] /= 2; }

void Chip8::_instr8XY7(byte X, byte Y) {
  if(_V[X] < _V[Y])
      _V[0xF] = 1;
    else _V[0xF] = 0;
    _V[X] = _V[Y] - _V[X]; }

void Chip8::_instr8XYE(byte X) {
  _V[0xF] = _V[X] & 0x80;
  _V[X] *= 2; }

void Chip8::_instr9XY0(byte X, byte Y) {
  if(_V[X] != _V[Y])
    _PC += 2; }

void  Chip8::_instrANNN(unsigned short NNN) {
  _I = NNN; }

void Chip8::_instrBNNN(unsigned short NNN) {
  _PC = NNN + _V[0]; }

void Chip8::_instrCXKK(byte X, byte KK) {
  byte r = m_RandGen() % 255;
  _V[X] = r & KK; }

void Chip8::_instrDXYK(byte X, byte Y, byte K) {
  // XOR Sprite over framebuffer
  bool collision = false;
  for(int i = 0 ; i != K ; ++i) {
      int xOffsetCounter = 0;
    for(int it = 0x80 ; it != 0 ; it /= 2) {
      if(xorPixel(_V[X] + xOffsetCounter, _V[Y] + i, _Memory[_I + i] & it))
        collision = true;
      ++xOffsetCounter; } }

  // And then, render
  SDL_Rect pix;
  pix.h = m_PixelSize; pix.w = m_PixelSize;
  for(int i = 0 ; i != 2048 ; ++i) {
    pix.x = (i % 64) * m_PixelSize;
    pix.y = i / 64 * m_PixelSize;
    if(_FrameBuffer[i])
      SDL_SetRenderDrawColor(m_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    else SDL_SetRenderDrawColor(m_SDLRenderer, 0, 0, 0, 0xFF);
    SDL_RenderFillRect(m_SDLRenderer, &pix); }
  SDL_RenderPresent(m_SDLRenderer);
  _V[0xF] = (collision) ? 1 : 0; }

void Chip8::_instrEX9E(byte X) {
  m_KeyStateMutex.lock();
  if(_KeyboardState[_V[X]])
     _PC+=2;
  m_KeyStateMutex.unlock(); }

void Chip8::_instrEXA1(byte X) {
  m_KeyStateMutex.lock();
  if(!_KeyboardState[_V[X]])
     _PC+=2;
  m_KeyStateMutex.unlock(); }

void Chip8::_instrFX07(byte X) {
  m_DTmutex.lock();
  _V[X] = _DT;
  m_DTmutex.unlock(); }

void Chip8::_instrFX0A(byte X) {
  m_KeyStateMutex.lock();
  // Find a way to pause execution til a key is pressed...
  m_KeyStateMutex.unlock(); }

void Chip8::_instrFX15(byte X) {
  m_DTmutex.lock();
  _DT = _V[X];
  m_DTmutex.unlock(); }

void Chip8::_instrFX18(byte X) {
  m_DTmutex.lock();
  _ST = _V[X];
  m_DTmutex.unlock(); }

void Chip8::_instrFX1E(byte X) {
  _I += _V[X]; }

void Chip8::_instrFX29(byte X) {
  _I = _V[X] * 5; }

void Chip8::_instrFX33(byte X) {
  _Memory[_I] = _V[X] / 100;
  _Memory[_I] = (_V[X] % 100) / 10;
  _Memory[_I] = _V[X] % 10; }

void Chip8::_instrFX55(byte X) {
  for(int i = 0 ; i != 0x10; ++i)
    _Memory[_I + i] = _V[i]; }

void Chip8::_instrFX65(byte X) {
  for(int i = 0 ; i != 0x10; ++i)
    _V[i] = _Memory[_I + i]; }
