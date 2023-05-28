#include "../inc/config.h"
#include "../inc/logging.h"
#include "../inc/constants.h"

//TODO: This will index non ROMs, use the typeAssoc data member in Emu to filter out irrelevant files, see line 15
void writeDirToRomConfig(std::vector<Emu>& emus, std::vector<Rom>& roms, std::string path) {

    fs::path dirPath = path;

    if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
        for (const auto & file : fs::directory_iterator(dirPath)) {
            const fs::path filePath = file.path();
            const std::string extension = toUpper(extractExtension(filePath));

            if(extension != "SAV") {
                std::string fullPath = fmt::format("{}{}", path, filePath.filename().c_str());
                writeRomToConfig(emus, roms, fullPath);
            }
        }
    }

    return;
}

std::string extractExtension(std::string path) {
    size_t lastDot = path.find_last_of(".");

    return path.substr(lastDot + 1, path.length());
}

std::string toUpper(std::string str) {
    for (auto & c: str) c = (unsigned char) toupper(c);
    return str;
}

void writeRomToConfig(std::vector<Emu>& emus, std::vector<Rom>& roms, std::string path) {
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
        .emulator = autoDetectEmu(emus, romExt),
    };

    roms.push_back(rom);
    return;
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
    Logger::log(fmt::format("Index result written to: {}", path), logSeverity::INFO);

    return;
}

std::vector<Emu> loadEmusFromConfig(std::string fileName) {
    YAML::Node configFile = YAML::LoadFile(fileName);
    std::vector<Emu> emu{};

    for (const auto& entry : configFile) {
        Emu newEmu{};
        newEmu.id = emu.size();
        newEmu.name = entry["name"].as<std::string>();
        newEmu.type = entry["type"].as<std::string>();
        newEmu.filename = entry["path"].as<std::string>();

        const YAML::Node& extensionNode = entry["extension"];
        for (const YAML::Node& node : extensionNode) {
            newEmu.typeAssoc.push_back(node.as<std::string>());
        }

        const YAML::Node& argsNode = entry["args"];
        for (const YAML::Node& node : argsNode) {
            newEmu.args.push_back(node.as<std::string>());
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

//TODO: Recursive indexing
std::vector<Rom> index(std::vector<Emu>& emus, std::string dir) {
    std::vector<Rom> roms{};

    writeDirToRomConfig(emus, roms, expandTilde(dir));

    #ifdef RELEASE
        writeRomConfigToFile(roms, DEFAULT_CONFIG_ROMS_YAML);
    #elif defined(DEBUG)
        writeRomConfigToFile(roms, "../config/test.yaml");
    #endif

    return roms;
}

std::string expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    std::string expandedPath;
    const char* homeDir = std::getenv("HOME");

    if (homeDir) {
        expandedPath = fmt::format("{}{}", homeDir, path.substr(1));
    } else {
        expandedPath = fmt::format("/home/username{}", homeDir, path.substr(1));
    }

    return expandedPath;
}

std::string autoDetectEmu(std::vector<Emu>& emus, std::string ext) {

    for(const auto& emu : emus) {
        for(const auto& ft : emu.typeAssoc) {
            if (toUpper(ft) == ext) {
                return emu.filename;
            }
        }
    }

    return "mgba-qt";
}