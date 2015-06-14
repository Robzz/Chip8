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
#include <iostream>
#include <exception>
#include <boost/log/trivial.hpp>
#include <SDL.h>
#include "Chip8.h"
#include "Chip8_config.h"
#include "errors.h"

using namespace std;

// Possible return values :
// 0 : execution OK
// 1 : Error loading SDL
// 2 : Error loading rom file
int main(int argc, char* argv[]) {
    cout << "Starting Chip8_PROGRAM_NAME v"  << Chip8_VERSION_MAJOR << '.' << Chip8_VERSION_MINOR << endl;
    Chip8* context;
    try {
        /*
        TODO
        It would be much cleaner to parse command line options here and not
        in Chip8's contructor.
        */
        context = new Chip8(argc, argv);
    }
    catch(Chip8Exception const& e) {
        cerr << "Error initializing Chip8_PROGRAM_NAME." << endl;
        BOOST_LOG_TRIVIAL(fatal) << e.what();
        return 1;
    }
    catch(exception e) {
        cerr << "An unknown exception occured." << endl;
        BOOST_LOG_TRIVIAL(fatal) << e.what();
        throw;
    }

    BOOST_LOG_TRIVIAL(info) << "Chip8 emulator initialized successfully!";

    context->run();

    delete context;
    return 0;
}
