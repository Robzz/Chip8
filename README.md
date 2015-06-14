# Chip8
A Chip8 emulator written in C++ that I made a couple years ago.
I am currently refactoring it a little. The code will remain ugly until I'm done.

Required libraries:
* Boost
    * log
    * program_options
    * filesystem
    * system
    * thread
* SDL2

The build system needs a working pkg-config that is able to locate the SDL2 library.
Builds on Windows and Linux. Untested on Mac OS.
