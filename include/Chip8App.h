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

#ifndef CHIP8APP_H
#define CHIP8APP_H

#include <wx/wx.h>
#include "Chip8.h"

class Chip8App : public wxApp
{
    public:
        Chip8App();
        virtual ~Chip8App();

        virtual bool onInit();

    private:
        Chip8* m_emu;
};

#endif // CHIP8APP_H
