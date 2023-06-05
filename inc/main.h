#ifndef MAIN_H
#define MAIN_H

#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <iostream>
#include <cxxopts.hpp>
#include <ncurses.h>
#include <spdlog/spdlog.h>
#include <filesystem>

#include "../inc/config.h"
#include "../inc/window.h"
#include "../inc/structs.h"
#include "../inc/constants.h"

void printRoms(std::vector<Rom>& roms);
void startEmulator(const Emu& emu, const Rom& rom);
std::string expandTilde(const std::string& path);

#endif // !MAIN_H