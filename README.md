# Chip8++
A Chip8 emulator written in C++ that I made a couple years ago to learn about emulation.
I am currently refactoring it a little and adding new features.
The code will probably remain ugly for a little while.

Required libraries:
* wxWidgets >= 3.0
* Boost
    * log
    * program_options
    * filesystem
    * system
    * thread
* SDL2

The build system needs a working pkg-config that is able to locate the SDL2 library.
Builds on Windows and Linux. Untested on Mac OS.
