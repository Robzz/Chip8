/*  Chip8++ - A CHIP-8 emulator.
    Copyright (C) 2015  Robin Chavignat

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */
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
