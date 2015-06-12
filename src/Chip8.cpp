#include "Chip8.h"
#include "Chip8_InitError.h"
#include "Chip8_FileException.h"
#include "Chip8_UnknownOpcode.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <string>
#include <boost/chrono.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

Chip8::Chip8(int argc, char* argv[])
try :
    m_PrintHelp(false),
    m_PixelSize(4),
    m_Chip8CpuFreq(1000),
    m_RomPath(),
    m_Dispatch(),
    m_SDLWindow(),
    m_SDLRenderer(),
    m_RandGen(chrono::system_clock::now().time_since_epoch().count()),
    _FrameBuffer(),
    _KeyboardState(),
    _CallStack(),
    _Memory(),
    _V(),
    _SP(0),
    _DT(0),
    _ST(0),
    _PC(0x0200),
    _I(0),
    m_KeepThreadsAlive(true),
    m_DTmutex(),
    m_KeyStateMutex(),
    m_CPUThread(),
    m_ClockThread() {
    // Initialize log first
    log::add_file_log("chip8.log");
    log::core::get()->set_filter(log::trivial::severity >= log::trivial::info);
    BOOST_LOG_TRIVIAL(info) << "Logging modules initialized, launching chip8!";

    m_ReadOptions(argc, argv);

    // Initialize SDL
    m_SDLWindow = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * m_PixelSize, 32 * m_PixelSize, SDL_WINDOW_HIDDEN);
    m_SDLRenderer = SDL_CreateRenderer(m_SDLWindow, -1, 0);

    if(!(m_SDLWindow && m_SDLRenderer)) {
        throw Chip8_InitError();
    }

    BOOST_LOG_TRIVIAL(info) << "SDL initialized successfully";

    // Initialize stack and register arrays
    for(int i = 0 ; i != 16 ; ++i) {
        _CallStack[i] = 0;
        _V[i] = 0;
    }

    // Initialize Sprite Data
    _Memory[0] = 240;  _Memory[1] = 144;  _Memory[2] = 144;  _Memory[3] = 144;
    _Memory[4] = 240;  _Memory[5] = 32;   _Memory[6] = 96;   _Memory[7] = 32;
    _Memory[8] = 32;   _Memory[9] = 112;  _Memory[10] = 240; _Memory[11] = 16;
    _Memory[12] = 240; _Memory[13] = 128; _Memory[14] = 240; _Memory[15] = 240;
    _Memory[16] = 16;  _Memory[17] = 240; _Memory[18] = 16;  _Memory[19] = 240;
    _Memory[20] = 144; _Memory[21] = 144; _Memory[22] = 240; _Memory[23] = 16;
    _Memory[24] = 16;  _Memory[25] = 240; _Memory[26] = 128; _Memory[27] = 240;
    _Memory[28] = 16;  _Memory[29] = 240; _Memory[30] = 240; _Memory[31] = 128;
    _Memory[32] = 240; _Memory[33] = 144; _Memory[34] = 240; _Memory[35] = 240;
    _Memory[36] = 16;  _Memory[37] = 32;  _Memory[38] = 64;  _Memory[39] = 64;
    _Memory[40] = 240; _Memory[41] = 144; _Memory[42] = 240; _Memory[43] = 144;
    _Memory[44] = 240; _Memory[45] = 240; _Memory[46] = 144; _Memory[47] = 240;
    _Memory[48] = 16;  _Memory[49] = 240; _Memory[50] = 240; _Memory[51] = 144;
    _Memory[52] = 240; _Memory[53] = 144; _Memory[54] = 144; _Memory[55] = 224;
    _Memory[56] = 144; _Memory[57] = 224; _Memory[58] = 144; _Memory[59] = 224;
    _Memory[60] = 240; _Memory[61] = 128; _Memory[62] = 128; _Memory[63] = 128;
    _Memory[64] = 240; _Memory[65] = 224; _Memory[66] = 144; _Memory[67] = 144;
    _Memory[68] = 144; _Memory[69] = 224; _Memory[70] = 240; _Memory[71] = 128;
    _Memory[72] = 240; _Memory[73] = 128; _Memory[74] = 240; _Memory[75] = 240;
    _Memory[76] = 128; _Memory[77] = 240; _Memory[78] = 128; _Memory[79] = 128;

    // Filling the rest with 0's
    for(int i = 80 ; i != 4096 ; ++i) {
        _Memory[i] = 0;
    }
} catch(Chip8_FileException const& e) {
    BOOST_LOG_TRIVIAL(fatal) << "Cannot initialize SDL : " << e.what();
    SDL_DestroyRenderer(m_SDLRenderer);
    SDL_DestroyWindow(m_SDLWindow);
} catch(Chip8_InitError e) {
    cout << "Error initialising SDL : " << e.what() << endl;
    if(m_SDLRenderer)
        SDL_DestroyRenderer(m_SDLRenderer);
    if(m_SDLWindow)
        SDL_DestroyWindow(m_SDLWindow);
} catch(...) {
    throw;
}

Chip8::~Chip8() noexcept {
    // Kill active threads
    m_KeepThreadsAlive = false;
    m_ClockThread.join();
    m_CPUThread.join();

    // Destroy SDL context and we should be good to go
    SDL_DestroyRenderer(m_SDLRenderer);
    SDL_DestroyWindow(m_SDLWindow);
}

void Chip8::m_ReadOptions(int argc, char** argv) {
    // Declare command line options
    program_options::options_description generic("Generic options");
    generic.add_options()("help", "Print help message")
    ("cfg-file", program_options::value<string>(), "Config file")
    ("rom", program_options::value<string>(), "ROM file to load");

    // And now declare config file options
    program_options::options_description config("Configuration options");
    config.add_options()("config.clock-freq", program_options::value<int>(), "CPU Clock frequency, in Hz")
    ("display.pixelSize", program_options::value<unsigned int>(), "Chip8 pixel size, in pixels");

    // Read and parse options
    program_options::variables_map vmap;
    program_options::store(program_options::parse_command_line(argc, argv, generic), vmap);
    program_options::notify(vmap);

    string cfgFileName = vmap.count("cfg-file") ? vmap["cfg-file"].as<string>() : "Chip8.ini" ;
    ifstream cfgFile(cfgFileName);
    if(!cfgFile.is_open()) {
        if(vmap.count("cfg-file"))
            throw Chip8_FileException("Couldn't load config file.");
    } else {
        cfgFile.seekg(0);
        program_options::store(program_options::parse_config_file(cfgFile, config), vmap);
    }
    program_options::notify(vmap);

    if(vmap.count("help")) {
        cout << generic << endl;
        m_PrintHelp = true;
    } else {
        if(!vmap.count("rom")) {
            throw Chip8_FileException("Please specify rom file! (option --rom)");
        } else
            m_RomPath = vmap["rom"].as<string>();

        // Config file options
        if(vmap.count("config.clock-freq"))
            m_Chip8CpuFreq = vmap["config.clock-freq"].as<int>();
        if(vmap.count("display.pixelSize"))
            m_PixelSize = vmap["display.pixelSize"].as<unsigned int>();
    }
}

int Chip8::loadRom() {
    ifstream romFile(m_RomPath, ios_base::in | ios_base::binary | ios_base::ate);
    if(!(romFile.is_open())) {
        throw Chip8_FileException(m_RomPath);
    }

    // Get rom size and load it at memory address 0x0200
    size_t romSize = romFile.tellg();
    romFile.seekg(0);
    romFile.read(reinterpret_cast<char*>(&(_Memory[0x0200])), romSize);
    romFile.close();

    BOOST_LOG_TRIVIAL(info) << "Rom " << m_RomPath << " successfully loaded!";

    return 0;
}

void Chip8::run() {
    if(m_PrintHelp) {
        // In that case, we don't actually run.
        return;
    }

    // Then we gotta load the ROM
    try {
        loadRom();
        m_CPUThread = thread(&Chip8::_CPUThread, this);
        m_ClockThread = thread(&Chip8::_ClockThread, this);
    } catch(Chip8_FileException const& e) {
        BOOST_LOG_TRIVIAL(error) << "Error loading rom " << e.what();
        return;
    } catch(thread_resource_error const& e) {
        BOOST_LOG_TRIVIAL(fatal) << "Couldn't create thread.";
        throw;
    } catch(...) {
        BOOST_LOG_TRIVIAL(fatal) << "Unexpected exception when trying to load the ROM.";
        throw;
    }

    bool end = false;
    SDL_Event event;

    // We can now show the SDL window, and hide it when the loop ends.
    SDL_ShowWindow(m_SDLWindow);

    while(!end) {
        SDL_WaitEvent(&event);
        if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) { // If a key was pressed or released...
            bool pressed = (event.type == SDL_KEYDOWN); // find out which one it was...
            m_KeyStateMutex.lock();
            switch(event.key.keysym.sym) { // and update keystate
            case SDLK_KP_0:
                _KeyboardState.set(0, pressed);
                break;
            case SDLK_KP_1:
                _KeyboardState.set(1, pressed);
                break;
            case SDLK_KP_2:
                _KeyboardState.set(2, pressed);
                break;
            case SDLK_KP_3:
                _KeyboardState.set(3, pressed);
                break;
            case SDLK_KP_4:
                _KeyboardState.set(4, pressed);
                break;
            case SDLK_KP_5:
                _KeyboardState.set(5, pressed);
                break;
            case SDLK_KP_6:
                _KeyboardState.set(6, pressed);
                break;
            case SDLK_KP_7:
                _KeyboardState.set(7, pressed);
                break;
            case SDLK_KP_8:
                _KeyboardState.set(8, pressed);
                break;
            case SDLK_KP_9:
                _KeyboardState.set(9, pressed);
                break;
            case SDLK_a:
                _KeyboardState.set(0xA, pressed);
                break;
            case SDLK_b:
                _KeyboardState.set(0xB, pressed);
                break;
            case SDLK_c:
                _KeyboardState.set(0xC, pressed);
                break;
            case SDLK_d:
                _KeyboardState.set(0xD, pressed);
                break;
            case SDLK_e:
                _KeyboardState.set(0xE, pressed);
                break;
            case SDLK_f:
                _KeyboardState.set(0xF, pressed);
                break;
            default:
                break;
            }
            m_KeyStateMutex.unlock();
        } else if(event.type == SDL_QUIT)
            end = true;
    }
    SDL_HideWindow(m_SDLWindow);
}

bool Chip8::xorPixel(int x, int y, bool val) {
    // Wrap around
    if(x >= 64 || y >= 32) {
        x %= 64;
        y %= 32;
    }

    bool curState = _FrameBuffer[x + 64 * y];
    _FrameBuffer.set(x + 64 * y, curState ^ val);

    // Collision detection
    if(curState && val) {
        return true;
    } else {
        return false;
    }
}

void Chip8::_ClockThread() {
    chrono::duration<long, micro> step(16666);
    while(m_KeepThreadsAlive) {
        if(_DT || _ST) {
            m_DTmutex.lock();
            if(_DT) {
                --_DT;
            }
            if(_ST) {
                --_ST;
            }
            m_DTmutex.unlock();
        }
        this_thread::sleep_for(step);
    }
}

void Chip8::_CPUThread() {
    byte lowByte, hiByte, code, X, Y, K;
    unsigned short NNN;

    // Compute clock cycle time
    chrono::duration<long long, micro> cycleDuration(1000000 / m_Chip8CpuFreq);

    while(m_KeepThreadsAlive) {
        chrono::high_resolution_clock::time_point instrStartTime = chrono::high_resolution_clock::now();
        lowByte = _Memory[_PC + 1];
        hiByte = _Memory[_PC];
        code = (hiByte & 0xF0) / 0x10;
        X = hiByte & 0x0F;
        Y = (lowByte & 0xF0) / 0x10;
        K = lowByte & 0x0F;
        NNN = (X * 0x100) + lowByte;
        _PC+=2;

        try {
            switch(code) {
            case 0:
                if(NNN == 0x0E0) {
                    _instr00E0();
                    break;
                } else if(NNN == 0x0EE) {
                    _instr00EE();
                    break;
                } else { // opcode 0nnn, ignored
                    break;
                }
            case 1:
                _instr1NNN(NNN);
                break;
            case 2:
                _instr2NNN(NNN);
                break;
            case 3:
                _instr3XKK(X, lowByte);
                break;
            case 4:
                _instr4XKK(X, lowByte);
                break;
            case 5:
                if(!K)
                    _instr5XY0(X, Y);
                break;
            case 6:
                _instr6XKK(X, lowByte);
                break;
            case 7:
                _instr7XKK(X, lowByte);
                break;
            case 8:
                switch(K) {
                case 0:
                    _instr8XY0(X, Y);
                    break;
                case 1:
                    _instr8XY1(X, Y);
                    break;
                case 2:
                    _instr8XY2(X, Y);
                    break;
                case 3:
                    _instr8XY3(X, Y);
                    break;
                case 4:
                    _instr8XY4(X, Y);
                    break;
                case 5:
                    _instr8XY5(X, Y);
                    break;
                case 6:
                    _instr8XY6(X);
                    break;
                case 7:
                    _instr8XY7(X, Y);
                    break;
                case 0xE:
                    _instr8XYE(X);
                    break;
                }
            case 9:
                if(!K)
                    _instr9XY0(X, Y);
                break;
            case 0xA:
                _instrANNN(NNN);
                break;
            case 0xB:
                _instrBNNN(NNN);
                break;
            case 0xC:
                _instrCXKK(X, lowByte);
                break;
            case 0xD:
                _instrDXYK(X, Y, K);
                break;
            case 0xE:
                if(lowByte == 0x9E) {
                    _instrEX9E(X);
                } else if(lowByte == 0xA1) {
                    _instrEXA1(X);
                }
                break;
            case 0xF:
                switch(lowByte) {
                case 0x07:
                    _instrFX07(X);
                    break;
                case 0x0A:
                    _instrFX0A(X);
                    break;
                case 0x15:
                    _instrFX15(X);
                    break;
                case 0x18:
                    _instrFX18(X);
                    break;
                case 0x1E:
                    _instrFX1E(X);
                    break;
                case 0x29:
                    _instrFX29(X);
                    break;
                case 0x33:
                    _instrFX33(X);
                    break;
                case 0x55:
                    _instrFX55(X);
                    break;
                case 0x65:
                    _instrFX65(X);
                    break;
                }
                break;
            default:
                throw Chip8_UnknownOpcode(hiByte * 0x100 + lowByte);
            }
            boost::this_thread::sleep_until(instrStartTime + cycleDuration);
        } catch(Chip8_UnknownOpcode e) {
            cout << "Warning : unknown opcode. Trying to continue..." << endl;
            BOOST_LOG_TRIVIAL(error) << e.what();
        } catch(...) {
            cout << "Unknown exception on CPU thread. Aborting.\nSee log file for details." << endl;
            BOOST_LOG_TRIVIAL(fatal) << "Unknown exception in CPU thread.";
        }
    }
}