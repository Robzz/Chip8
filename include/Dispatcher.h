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
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <map>

class Dispatcher
{
  public:
    Dispatcher();
    virtual ~Dispatcher() noexcept;
    void operator()();
    unsigned int getCounter() const noexcept;
    void resetCounter() noexcept;

  private:
    unsigned int m_instrCount;
    //std::map<> m_FuncMap;
};

#endif // DISPATCHER_H
