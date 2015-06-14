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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Chip8Config
{
public:
    ~Chip8Config();

    std::string romName() const;
    int pixelSize() const;
    int cpuFreq() const;
    bool run() const;

    static Chip8Config* parseCommandLineAndCfg(int argc, char** argv);
    static void writeConfigFile(std::string fileName);

protected:
private:
    Chip8Config(bool run, std::string const& romName, int pixelSize, int cpuFreq);
    std::string m_romName;
    int m_pixelSize;
    int m_cpuFreq;
    bool m_run;
};


#endif // CONFIG_H

