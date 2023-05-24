#ifndef MAIN_H
#define MAIN_H

#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct RomStruct {
    short id{};
    std::string name;
    std::string filename;
    std::string type;
    std::string emulator;
};

using Rom = RomStruct;

//TODO: For a GUI, let's use ncurses
//TODO: Write function that writes to the config file
void printRoms(std::vector<Rom>& roms);
void startEmulator(Rom rom);
std::string expandTilde(const std::string& path);

#endif