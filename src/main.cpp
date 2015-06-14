#include <iostream>
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
    cout << "Starting Chip8 v"  << Chip8_VERSION_MAJOR << '.' << Chip8_VERSION_MINOR << endl;
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
        // TODO : why the fuck did I print on the screen? Do i not have a friggin' logger?
        cerr << "Error initializing the Chip8 emulator." << endl;
        return 1;
    }
    catch(...) {
        cerr << "An unknown exception occured." << endl;
        throw;
    }

    BOOST_LOG_TRIVIAL(info) << "Chip8 emulator initialized successfully!";

    context->run();

    delete context;
    return 0;
}
