#include "../inc/main.h"
#include "../inc/logging.h"
#include "../inc/config.h"
#include "../inc/window.h"

#define DEBUG

#include <cxxopts.hpp>

cxxopts::ParseResult setUpWorkflow(int argc, char** argv, cxxopts::Options & options);
void printVersion();
void initWindow();
std::vector<Rom> index(std::string dir);

std::vector<Rom> index(std::string dir) {
    std::vector<Rom> roms{};

    writeDirToRomConfig(roms, expandTilde(dir));

    #ifdef RELEASE
    writeRomConfigToFile(roms, "~/.config/romManager/roms.yaml");
    #elif defined(DEBUG)
    writeRomConfigToFile(roms, "../config/test.yaml");
    #endif

    return roms;
}

//TODO: Search/Filtering option
//TODO: Save file management? After above stuff is finished
//TODO: Idea: for save file management maybe I can let saves be saved to the RomManager dir and then copy to desired folder
int main(int argc, char** argv) {
    Logger::setAllowedToPrint(true);
    cxxopts::Options options("romManager", "A simple ncurses driven rom manager, for all your emulation needs!\nPlease only use this tool with legally obtained roms.");
    auto optRes = setUpWorkflow(argc, argv, options);

    #ifdef RELEASE
    std::string path = expandTilde("~/.config/romManager/roms.yaml");
    #elif defined(DEBUG)
    std::string path = "../config/test.yaml";
    #endif

    std::vector<Rom> roms{};
    std::vector<Emu> emus{};


    // try {
    //     emus = loadEmusFromConfig(path);
    // }
    // catch(YAML::InvalidNode& error) {
    //      Logger::log(fmt::format("Could not parse the emulator config file, reason: {}", error.msg), logSeverity::ERROR);
    //      return -1;
    // }
    // catch(YAML::BadFile& error) {
    //     Logger::log(fmt::format("Could not load emulator config file, reason: {}", error.msg), logSeverity::ERROR);
    //     return -1;
    // }

    try {
        roms = loadRomsFromConfig(path);
    }
    catch(YAML::BadFile& error) {
        Logger::log(fmt::format("Could not load rom config file, reason: {}", error.msg), logSeverity::ERROR);
        return -1;
    }

    if (roms.size() == 0) {
        Logger::log("Config file empty!\n", logSeverity::ERROR);
        return -1;
    }

    Menu menu(15, 90, 0, 0, roms);
    menu.OnInit();
    menu.OnExecute();
    menu.OnCleanup();
       
    return 0;
}

cxxopts::ParseResult setUpWorkflow(int argc, char** argv, cxxopts::Options & options) {
   options.add_options()
        ("i,index", "INDEX Rom directory")
        ("v,version", "PRINT program version")
        ("h,help", "PRINT help text")
    ; 

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        fmt::print("{}", options.help());
        exit(EXIT_SUCCESS);
    }

    if (result.count("version")) {
        printVersion();
        exit(EXIT_SUCCESS);
    }

    if (result.count("index")) {
        std::string dirToIndex =  expandTilde("~/AllRoms/");

        Logger::log(fmt::format("Indexing: {}", dirToIndex), logSeverity::INFO);
        auto ret = index(dirToIndex);
        Logger::log(fmt::format("Finished indexing {}", dirToIndex), logSeverity::INFO);
        Logger::log(fmt::format("Roms found: {}", ret.size()), logSeverity::INFO);
        exit(EXIT_SUCCESS);
    }

    return result;
}

void printVersion() {
    fmt::print("romManager VERSION {}\n", fmt::styled("0.12", fmt::fg(fmt::color::orange)));
}

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
        expandedPath = fmt::format("{}{}", homeDir, path.substr(1));
    } else {
        expandedPath = fmt::format("/home/username{}", homeDir, path.substr(1));
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
