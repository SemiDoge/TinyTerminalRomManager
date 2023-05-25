#ifndef CONFIG_H
#define CONFIG_H

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include "main.h"


struct EmuStruct {
    short id{};
    std::string name;
    std::string filename;
    std::vector<std::string> typeAssoc;
    std::string emulator;

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const EmuStruct& emu) {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << emu.name;
        out << YAML::Key << "extension" << YAML::Value << emu.typeAssoc;
        out << YAML::Key << "type" << YAML::Value << emu.emulator;
        out << YAML::Key << "path" << YAML::Value << emu.filename;
        out << YAML::EndMap;

        return out;
    }
};

using Emu = EmuStruct;
namespace fs = std::filesystem;

std::string toUpper(std::string str);
std::string extractExtension(std::string path);
void writeDirToRomConfig(std::vector<Rom>& roms, std::string path);
void writeRomToConfig(std::vector<Rom>& roms, std::string path);
void writeRomConfigToFile(std::vector<Rom>& roms, std::string path);
void writeEmuConfigToFile(std::vector<Emu>& emus, std::string path);
std::vector<Rom> loadRomsFromConfig(std::string fileName);
std::vector<Emu> loadEmusFromConfig(std::string fileName);

#endif