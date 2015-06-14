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
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "errors.h"
#include "Chip8Config.h"

using namespace std;
namespace po = boost::program_options;

Chip8Config::Chip8Config(bool run, string const& romName, int pixelSize, int cpuFreq) :
    m_romName(romName),
    m_pixelSize(pixelSize),
    m_cpuFreq(cpuFreq),
    m_run(run)
{ }

Chip8Config::~Chip8Config()
{ }

string Chip8Config::romName() const {
    return m_romName;
}

int Chip8Config::pixelSize() const {
    return m_pixelSize;
}

int Chip8Config::cpuFreq() const {
    return m_cpuFreq;
}

bool Chip8Config::run() const {
    return m_run;
}

Chip8Config* Chip8Config::parseCommandLineAndCfg(int argc, char** argv) {
    cout << "Beginning to parse config" << endl;
    Chip8Config* res = nullptr;
    bool _run = true;

    // Declare command line options
    po::options_description generic("Generic options");
    generic.add_options()
        ("help", "Print help message")
        ("cfg-file", po::value<string>(), "Config file")
        ("rom", po::value<string>(), "ROM file to load")
        ("create-cfg", po::value<string>()->implicit_value("Chip8.ini"),
            "Create a new configuration file.");

    // And now declare config file options
    po::options_description config("Configuration options");
    config.add_options()
        ("config.cpufreq", po::value<int>(), "CPU Clock frequency, in Hz")
        ("display.pixelSize", po::value<int>(), "Chip8 pixel size, in pixels");

    // Read and parse options
    po::variables_map vmap;
    po::store(po::parse_command_line(argc, argv, generic), vmap);
    po::notify(vmap);

    string cfgFileName = vmap.count("cfg-file") ? vmap["cfg-file"].as<string>() : "Chip8.ini" ;
    ifstream cfgFile(cfgFileName, ios_base::in);
    if(!cfgFile.is_open()) {
        if(vmap.count("cfg-file"))
            throw Chip8FileError("Couldn't load config file.");
    }
    else {
        cfgFile.seekg(0);
        po::store(po::parse_config_file(cfgFile, config), vmap);
    }
    po::notify(vmap);

    if(vmap.count("help")) {
        cout << generic << endl;
        _run = false;
    }
    else if(vmap.count("create-cfg")) {
        writeConfigFile(vmap["create-cfg"].as<string>());
        _run = false;
    }
    else {
        string romFile;
        // TODO : Figure a way to use program_options default values in this case
        // (the problem is that when no file is read, the default values are not stored)
        int _cpuFreq = 1000, _pixelSize = 4;
        if(!vmap.count("rom")) {
            throw Chip8InitError("Please specify rom file! (option --rom)");
        } else
            romFile = vmap["rom"].as<string>();

        // Config file options
        if(vmap.count("config.cpufreq"))
            _cpuFreq = vmap["config.cpufreq"].as<int>();
        if(vmap.count("display.pixelSize"))
            _pixelSize = vmap["display.pixelSize"].as<int>();
        res = new Chip8Config(_run, romFile, _pixelSize, _cpuFreq);
    }
    return res;
}

void Chip8Config::writeConfigFile(string fileName) {
    // This function courtesy of StackOverflow :)
    // https://stackoverflow.com/questions/4703134/is-there-a-way-to-print-config-file-for-boost-program-options
    using boost::property_tree::ptree;

    ptree root;

    ptree config, display;
    // TODO : this is error prone, find a better way

    config.put("cpufreq", "1000");
    display.put("pixelSize", "4");

    root.push_front(ptree::value_type("config", config));
    root.push_front(ptree::value_type("display", display));

    ofstream file(fileName);
    write_ini(file, root);
}
