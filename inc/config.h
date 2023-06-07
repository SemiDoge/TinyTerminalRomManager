#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <spdlog/spdlog.h>

#include "structs.h"

namespace fs = std::filesystem;

void writeDirToRomConfig(std::vector<Emu>& emus, std::vector<Rom>& roms, std::string path);
void writeRomToConfig(std::vector<Emu>& emus, std::vector<Rom>& roms, const std::string& path);
void writeRomConfigToFile(std::vector<Rom>& roms, std::string path);
void writeEmuConfigToFile(std::vector<Emu>& emus, std::string path);
std::vector<Rom> loadRomsFromConfig(const std::string& fileName);
std::vector<Emu> loadEmusFromConfig(const std::string& fileName);
void index(std::vector<Emu>& emus, std::vector<Rom>& roms, const fs::path& dir, int depth, int max_depth);
std::string autoDetectEmu(std::vector<Emu>& emus, const std::string& ext);

#endif // !CONFIG_H