#include "../inc/main.h"

cxxopts::ParseResult setUpWorkflow(int argc, char** argv, cxxopts::Options & options);
void printVersion();

//TODO: Set up testing
//TODO: Search/Filtering option
//TODO: Save file management? After above stuff is finished
//TODO: Idea: for save file management maybe I can let saves be saved to the RomManager dir and then copy to desired folder
int main(int argc, char** argv) {
    Logger::setAllowedToPrint(true);
    cxxopts::Options options("romManager", 
        "A simple ncurses driven ROM manager, for all your emulation needs!\nPlease use this tool with only legally obtained ROMS."
    );
    auto optRes = setUpWorkflow(argc, argv, options);

    #ifdef RELEASE
    std::string romsPath = DEFAULT_CONFIG_ROMS_YAML;
    std::string emuPath =  DEFAULT_CONFIG_EMUS_YAML;
    #elif defined(DEBUG)
    std::string romsPath = "../config/test.yaml";
    std::string emuPath = "../config/emus.yaml";
    #endif

    std::vector<Rom> roms{};
    std::vector<Emu> emus{};


    try {
        emus = loadEmusFromConfig(emuPath);
    }
    catch(YAML::InvalidNode& error) {
         Logger::log(fmt::format("Could not parse the emulator config file, reason: {}", error.msg), logSeverity::ERROR);
         return -1;
    }
    catch(YAML::BadFile& error) {
        Logger::log(fmt::format("Could not load emulator config file, reason: {}", error.msg), logSeverity::ERROR);
        return -1;
    }

    try {
        roms = loadRomsFromConfig(romsPath);
    }
    catch(YAML::BadFile& error) {
        Logger::log(fmt::format("Could not load rom config file, reason: {}", error.msg), logSeverity::ERROR);
        return -1;
    }

    if (roms.size() == 0) {
        Logger::log("Config file empty!\n", logSeverity::ERROR);
        return -1;
    }

    Menu menu(roms.size(), 90, 0, 0, roms, emus);
    menu.OnInit();
    menu.OnExecute();
    menu.OnCleanup();
       
    return EXIT_SUCCESS;
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
        std::vector<Emu> emus{};
        #ifdef RELEASE
            std::string romsPath = DEFAULT_CONFIG_ROMS_YAML;
            std::string emuPath =  DEFAULT_CONFIG_EMUS_YAML;
        #elif defined(DEBUG)
            std::string romsPath = "../config/test.yaml";
            std::string emuPath = "../config/emus.yaml";
        #endif

        try {
            emus = loadEmusFromConfig(emuPath);
        }
        catch(YAML::InvalidNode& error) {
            Logger::log(fmt::format("Could not parse the emulator config file, reason: {}", error.msg), logSeverity::ERROR);
            exit(-1);
        }
        catch(YAML::BadFile& error) {
            Logger::log(fmt::format("Could not load emulator config file, reason: {}", error.msg), logSeverity::ERROR);
            exit(-1);
        }

        std::string dirToIndex =  expandTilde("~/RomManager/");

        Logger::log(fmt::format("Indexing: {}", dirToIndex), logSeverity::INFO);
        auto ret = index(emus, dirToIndex);
        Logger::log(fmt::format("Finished indexing {}", dirToIndex), logSeverity::INFO);
        Logger::log(fmt::format("Roms found: {}", ret.size()), logSeverity::INFO);
        exit(EXIT_SUCCESS);
    }

    return result;
}

void printVersion() {
    fmt::print("romManager VERSION {}.{}.{}\n", 
        fmt::styled(__VER__MAJOR__, fmt::fg(fmt::color::orange)),
        fmt::styled(__VER__MINOR__, fmt::fg(fmt::color::orange)),
        fmt::styled(__VER__PATCH__, fmt::fg(fmt::color::orange))
    );
}

void printRoms(std::vector<Rom>& roms) {
    for (const auto& rom : roms) {
        fmt::print("{:02X}: {} via {} ({})\n", rom.id, rom.name, rom.emulator, rom.type);
    }

    return;
}

void startEmulator(Emu emu, Rom rom) {
    pid_t pid = fork();

    if (pid == -1) {
        fmt::print("Fork failed!\n");
        return;
    } else if (pid == 0) {
        std::string command = rom.emulator;
        std::string arg = rom.filename;

        std::vector<char*> args;
        args.push_back(const_cast<char*>(command.c_str()));


        // TODO: This should check that if emu.args.size() > 0, ${FILENAME} ought to be in the emu args array
        if (emu.args.size() > 0) {
            for(const auto& emuArgs : emu.args) {
                if (emuArgs == "${FILENAME}") {
                    args.push_back(const_cast<char*>(rom.filename.c_str()));
                    continue;
                }

                args.push_back(const_cast<char*>(emuArgs.c_str()));
            } 

            args.push_back(nullptr);
            execvp(args[0], args.data());

        } else {
            std::string fileArg = expandTilde(arg);
            args.push_back(const_cast<char*>(fileArg.c_str()));
            args.push_back(nullptr);
            execvp(args[0], args.data());
        }

        fmt::print("Error executing command!\n");
        return;
    }
}

// I hate this function, design forced it
Emu chooseEmu(std::vector<Emu> emus, std::string path) {
    for(const auto& emu: emus) {
        if(emu.filename == path) {
            return emu;
        }
    }

    return emus[0];
}