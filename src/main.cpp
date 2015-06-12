#include <iostream>
#include <boost/log/trivial.hpp>
#include <SDL2/SDL.h>
#include "Chip8.h"
#include "Chip8_config.h"
#include "Chip8_FileException.h"

using namespace std;

// Possible return values :
// 0 : execution OK
// 1 : Error loading SDL
// 2 : Error loading rom file
int main(int argc, char* argv[]) {
    cout << "Starting Chip8 v"  << Chip8_VERSION_MAJOR << '.' << Chip8_VERSION_MINOR << endl;
    Chip8* context;
    try {
        context = new Chip8(argc, argv);
    }
    catch(Chip8_Exception const& e) {
        cerr << "Error loading SDL" << endl;
        return 1;
    }
    catch(...) {
        cerr << "Unknown exception" << endl;
        throw;
    }

    BOOST_LOG_TRIVIAL(info) << "Chip8 emu initialized successfully!";

    context->run();

    delete context;
    return 0;
}
