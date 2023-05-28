#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

struct EmuStruct {
    short id{};
    std::string name;
    std::string filename;
    std::vector<std::string> typeAssoc;
    std::string type;
    std::vector<std::string> args;

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const EmuStruct& emu) {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << emu.name;
        out << YAML::Key << "extension" << YAML::Value << emu.typeAssoc;
        out << YAML::Key << "type" << YAML::Value << emu.type;
        out << YAML::Key << "path" << YAML::Value << emu.filename;
        out << YAML::Key << "args" << YAML::Value << emu.args;
        out << YAML::EndMap;

        return out;
    }
};

struct RomStruct {
    short id{};
    std::string name;
    std::string filename;
    std::string type;
    std::string emulator;

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const RomStruct& rom) {
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << rom.name;
        out << YAML::Key << "filename" << YAML::Value << rom.filename;
        out << YAML::Key << "type" << YAML::Value << rom.type;
        out << YAML::Key << "emulator" << YAML::Value << rom.emulator;
        out << YAML::EndMap;

        return out;
    }
};

using Rom = RomStruct;
using Emu = EmuStruct;

#endif // !STRUCTS_H
