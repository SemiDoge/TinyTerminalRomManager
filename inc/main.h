#ifndef MAIN_H
#define MAIN_H

#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ncurses.h>

#define KEY_COMBO_CTRL_D 4
#define CTRL_D_STEP 3

struct RomStruct {
    short id{};
    std::string name;
    std::string filename;
    std::string type;
    std::string emulator;
};

using Rom = RomStruct;

void initWindow();
void printRoms(std::vector<Rom>& roms);
void startEmulator(Rom rom);
std::string expandTilde(const std::string& path);

#endif