#include "../inc/main.h"
#include "../inc/logging.h"
#include "../inc/config.h"

int main() {
    std::string path = "../config/roms.yaml";
    Logger::setAllowedToPrint(true);

    std::vector<Rom> roms{};

    try {
        roms = loadRomsFromConfig(path);
    }
    catch(YAML::BadFile& error) {
        Logger::log(fmt::format("Could not load config file, reason: {}", error.msg), logSeverity::ERROR);
        return -1;
    }

    Logger::log(fmt::format("Loaded config file: {}", path), logSeverity::INFO);
    printRoms(roms);
    Logger::log(fmt::format("Now playing: {}\n", roms[0].name), logSeverity::INFO);
    startEmulator(roms[0]);
    
    return 0;
}



//TODO: Build a system that assign's ID's to all of the roms and allows you to enter an ID to start the game
void printRoms(std::vector<Rom>& roms) {
    for (const auto& rom : roms) {
        fmt::print("{:02X}: {} via {} ({})\n", rom.id, rom.name, rom.emulator, rom.type);
    }

    return;
}

std::string expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    std::string expandedPath;
    const char* homeDir = std::getenv("HOME");

    if (homeDir) {
        expandedPath = homeDir + path.substr(1);
    } else {
        expandedPath = "/home/username" + path.substr(1);
    }

    return expandedPath;
}

void startEmulator(Rom rom) {
    pid_t pid = fork();

    if (pid == -1) {
        fmt::print("Fork failed!\n");
        return;
    } else if (pid == 0) {
        std::string command = rom.emulator;
        std::string arg = rom.filename;

        std::vector<char*> args;
        args.push_back(const_cast<char*>(command.c_str()));

        std::string expArg = expandTilde(arg);
        args.push_back(const_cast<char*>(expArg.c_str()));
        args.push_back(nullptr);

        execvp(args[0], args.data());

        fmt::print("Error executing command!\n");
        return;
    }
}
