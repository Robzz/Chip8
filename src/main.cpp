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
        /*
        TODO
        It would be much cleaner to parse command line options here and not
        in Chip8's contructor.
        */
        context = new Chip8(argc, argv);
    }
    /*
    TODO
    Also, merge the error classes in a single file. Or at the very least, in a separate
    directory.
    */
    catch(Chip8_Exception const& e) {
        // TODO : why the fuck did I print on the screen? Do i not have a friggin' logger?
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
