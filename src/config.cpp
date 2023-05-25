#include "../inc/config.h"
#include "../inc/logging.h"


#include <iostream>

void writeDirToRomConfig(std::vector<Rom>& roms, std::string path) {

    fs::path dirPath = path;

    if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
        for (const auto & file : fs::directory_iterator(dirPath)) {
            const fs::path filePath = file.path();
            const std::string extension = toUpper(extractExtension(filePath));

            if(extension == "GBA") {
                // std::string fp = fmt::format("{}{}", dirPath, filePath);
                std::string fullPath = fmt::format("{}{}", path, filePath.filename().c_str());
                writeRomToConfig(roms, fullPath);
            }
        }
    }
}

std::string extractExtension(std::string path) {
    size_t lastDot = path.find_last_of(".");

    return path.substr(lastDot + 1, path.length());
}

std::string toUpper(std::string str) {
    for (auto & c: str) c = (unsigned char) toupper(c);
    return str;
}

void writeRomToConfig(std::vector<Rom>& roms, std::string path) {
    size_t lastSlash = path.find_last_of("/\\");
    size_t lastDot = path.find_last_of(".");

    std::string romName{};
    std::string romExt{};

    //extract name
    romName = path.substr(lastSlash + 1, lastDot - (lastSlash + 1));
    romExt = extractExtension(path);

    std::transform(romExt.begin(), romExt.end(), romExt.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    
    Rom rom = {
        .name = romName,
        .filename = path,
        .type = romExt,
        //TODO: Write an autodetect function that looks up file extension associations in EmuStruct and assigns a emulator P
        .emulator = "mgba-qt",
    };

    roms.push_back(rom);
}

void writeRomConfigToFile(std::vector<Rom>& roms, std::string path) {
    std::ofstream file(path);

    YAML::Emitter emit;
    emit << YAML::BeginSeq;
    for (const auto& rom: roms) {
        emit << rom;
    }
    emit << YAML::EndSeq;
    file << emit.c_str();

    file.close();
}

std::vector<Emu> loadEmusFromConfig(std::string fileName) {
    YAML::Node configFile = YAML::LoadFile(fileName);
    std::vector<Emu> emu{};

    for (const auto& entry : configFile) {
        Emu newEmu{};
        newEmu.id = 0;
        newEmu.name = entry["name"].as<std::string>();
        newEmu.emulator = entry["type"].as<std::string>();
        newEmu.filename = entry["path"].as<std::string>();

        const YAML::Node& extensionNode = entry["extension"];
        for (const YAML::Node& node : extensionNode) {
            newEmu.typeAssoc.push_back(node.as<std::string>());
        }

        emu.push_back(newEmu);
    }

    return emu;
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