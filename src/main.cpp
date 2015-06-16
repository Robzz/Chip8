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
#include <boost/log/utility/setup/file.hpp>
#include <SDL.h>

#include "Chip8.h"
#include "Chip8_config.h"
#include "Chip8App.h"
#include "errors.h"

wxIMPLEMENT_APP_NO_MAIN(Chip8App);

using namespace std;
using namespace boost;

// Possible return values :
// 0 : execution OK
// 1 : Error loading SDL
// 2 : Error loading rom file
int main(int argc, char* argv[]) {
    log::add_file_log("chip8.log");
    log::core::get()->set_filter(log::trivial::severity >= log::trivial::info);
    BOOST_LOG_TRIVIAL(info) << "Logging modules initialized, launching chip8++!";
    cout << "Starting Chip8_PROGRAM_NAME v"  << Chip8_VERSION_MAJOR << '.' << Chip8_VERSION_MINOR << endl;

    // Initialize wxWidgets
    if(!wxEntryStart(argc, argv)) {
        BOOST_LOG_TRIVIAL(fatal) << "Cannot initialize wxWidgets, aborting...";
        cerr << "Fatal initialization error, see log file for details." << endl;
        return 1;
    }
    BOOST_LOG_TRIVIAL(info) << "wxWidgets initialized succesfully.";

    Chip8* context;
    try {
        Chip8Config* cfg = Chip8Config::parseCommandLineAndCfg(argc, argv);
        if(!cfg || !cfg->run())
            return 0;
        context = new Chip8(cfg);
        BOOST_LOG_TRIVIAL(info) << "Chip8 emulator initialized successfully!";

        context->run();

        delete context;
    } catch(Chip8InitError const& e) {
        cerr << "Error initializing Chip8_PROGRAM_NAME." << endl;
        BOOST_LOG_TRIVIAL(fatal) << e.what();
        return 1;
    } catch(std::exception e) {
        cerr << "An unknown exception occured, see log file for details." << endl;
        BOOST_LOG_TRIVIAL(fatal) << e.what();
        throw;
    } catch(...) {
        cerr << "An unknown exception occured." << endl;
        BOOST_LOG_TRIVIAL(fatal) << "Unexpected exception.";
        throw;
    }

    wxEntryCleanup();
    return 0;
}
