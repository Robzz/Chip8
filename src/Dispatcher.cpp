#include "Chip8_Typedefs.h"
#include "Dispatcher.h"
#include <cstdarg>
#include <boost/chrono.hpp>

using namespace boost::chrono;

typedef duration<long long, microseconds> instr_duration;
typedef time_point<high_resolution_clock> hr_time_point;

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
