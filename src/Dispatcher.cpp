#include "Chip8_Typedefs.h"
#include "Dispatcher.h"
#include <cstdarg>
#include <chrono>

using namespace std;

typedef chrono::time_point<chrono::high_resolution_clock> hr_time_point;

/*
TODO
Why did I make a class this useless?
*/

Dispatcher::Dispatcher() : m_instrCount(0)
{ }

Dispatcher::~Dispatcher() noexcept
{ }

void Dispatcher::resetCounter() noexcept {
  m_instrCount = 0; }

unsigned int Dispatcher::getCounter() const noexcept {
  return m_instrCount; }

void Dispatcher::operator()(void) {
  //hr_time_point tStart = high_resolution_clock.now();
  ++m_instrCount; }
