#include "../inc/config.h"


// TODO: Complete this function, that will basically be a scanner for roms in a directory
void writeRomToConfig(Rom rom) {
    assert("Unimplemented");
}

std::vector<Rom> loadRomsFromConfig(std::string fileName) {
    YAML::Node configFile = YAML::LoadFile(fileName);
    std::vector<Rom> roms{};

    short i = 0;
    for (const auto& entry : configFile) {
        Rom newRom{};
        newRom.id = i;
        newRom.name = entry["name"].as<std::string>();
        newRom.filename = entry["filename"].as<std::string>();
        newRom.type = entry["type"].as<std::string>();
        newRom.emulator = entry["emulator"].as<std::string>();

        roms.push_back(newRom);
        i++;
    }

    return roms;
}