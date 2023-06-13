#include "../inc/config.h"
#include "../inc/constants.h"

void writeRomToConfig(std::vector<Emu>& emus, std::vector<Rom>& roms, const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    size_t lastDot = path.find_last_of('.');

    std::string romName{};
    std::string romExt{};

    //extract name
    romName = path.substr(lastSlash + 1, lastDot - (lastSlash + 1));
    romExt = extractExtension(path);

    std::transform(romExt.begin(), romExt.end(), romExt.begin(), [](unsigned char chr) {
        return std::toupper(chr);
    });
    
    Rom rom = {
        .name = romName,
        .filename = path,
        .type = romExt,
        .emulator = autoDetectEmu(emus, romExt),
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
    spdlog::info("Index result written to: {}", path);
}

std::vector<Emu> loadEmusFromConfig(const std::string& fileName) {
    YAML::Node configFile = YAML::LoadFile(fileName);
    std::vector<Emu> emu{};

    for (const auto& entry : configFile) {
        Emu newEmu{};
        newEmu.name = entry["name"].as<std::string>();
        newEmu.type = entry["type"].as<std::string>();
        newEmu.filename = entry["path"].as<std::string>();
        //TODO: Need a way of creating an error if filename doesn't exist in emu config file.

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

std::vector<Rom> loadRomsFromConfig(const std::string& fileName) {
    YAML::Node configFile = YAML::LoadFile(fileName);
    std::vector<Rom> roms{};

    for (const auto& entry : configFile) {
        Rom newRom{};
        newRom.name = entry["name"].as<std::string>();
        newRom.filename = entry["filename"].as<std::string>();
        newRom.type = entry["type"].as<std::string>();
        newRom.emulator = entry["emulator"].as<std::string>();

        roms.push_back(newRom);
    }

    return roms;
}

void index(std::vector<Emu>& emus, std::vector<Rom>& roms, const fs::path& dir, int depth, int max_depth) {
    if (depth >= max_depth) {
        return;
    }

    spdlog::info("Indexing -> {}", dir.string());

    //TODO: This is called every directory, consider caching by doing this outside of index()
    std::unordered_set<std::string> validExtensions{};
    for(const auto& emu : emus) {
        for(const auto& ext: emu.typeAssoc) {
            validExtensions.insert(toUpper(ext));
        }
    }

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (fs::is_directory(entry)) {
            index(emus, roms, entry, depth + 1, max_depth);
        } else if (fs::is_regular_file(entry)) {
            const std::string extension = toUpper(extractExtension(entry.path().string()));

            for(const auto& ext : validExtensions) {
                if(ext == extension) {
                    writeRomToConfig(emus, roms, entry.path().string());
                    break;
                }
            }
        }
    }
}

std::string autoDetectEmu(std::vector<Emu>& emus, const std::string& ext) {

    for(const auto& emu : emus) {
        for(const auto& filetype : emu.typeAssoc) {
            if (toUpper(filetype) == ext) {
                return emu.filename;
            }
        }
    }

    return "";
}