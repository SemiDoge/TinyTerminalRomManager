#ifndef CONFIG_H
#define CONFIG_H

#include <assert.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "structs.h"

namespace fs = std::filesystem;

std::string toUpper(std::string str);
std::string extractExtension(std::string path);
void writeDirToRomConfig(std::vector<Rom>& roms, std::string path);
void writeRomToConfig(std::vector<Rom>& roms, std::string path);
void writeRomConfigToFile(std::vector<Rom>& roms, std::string path);
void writeEmuConfigToFile(std::vector<Emu>& emus, std::string path);
std::vector<Rom> loadRomsFromConfig(std::string fileName);
std::vector<Emu> loadEmusFromConfig(std::string fileName);
std::vector<Rom> index(std::string dir);
std::string expandTilde(const std::string& path);

#endif