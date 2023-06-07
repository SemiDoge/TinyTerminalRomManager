#include "../inc/main.h"

cxxopts::ParseResult setUpWorkflow(int argc, char** argv, cxxopts::Options & options);
void printVersion();


//TODO: Set up testing
//TODO: Save file management? After above stuff is finished
//TODO: Idea: for save file management maybe I can let saves be saved to the RomManager dir and then copy to desired folder
int main(int argc, char** argv) {
    spdlog::set_pattern("%^%l%$: %v");

    #if defined(DEBUG)
        spdlog::set_level(spdlog::level::debug);
    #endif // DEBUG

    cxxopts::Options options("ttrm", 
        "A simple ncurses driven ROM manager, for all your emulation management needs!\nPlease use this tool with only legally obtained ROMS."
    );

    try {
        const auto optRes = setUpWorkflow(argc, argv, options);
    } catch (...) {
        try {
            std::rethrow_exception(std::current_exception());
        } catch (const std::exception& error) {
            spdlog::error("{}", error.what());
            return EXIT_FAILURE;
        }
    }

    #ifdef RELEASE
        const std::string romsPath = DEFAULT_CONFIG_ROMS_YAML;
        const std::string emuPath =  DEFAULT_CONFIG_EMUS_YAML;
    #elif defined(DEBUG)
        const std::string romsPath = "../config/roms.yaml";
        const std::string emuPath = "../config/emus.yaml";
    #endif

    std::vector<Rom> roms{};
    std::vector<Emu> emus{};

    try {
        emus = loadEmusFromConfig(emuPath);
    } catch(YAML::InvalidNode& error) {
        spdlog::error("Could not parse the emulator config file, reason: {}", error.msg);
        return EXIT_FAILURE;
        
    } catch(YAML::BadFile& error) {
        spdlog::error("Could not load emulator config file, reason: {}", error.msg);
        return EXIT_FAILURE;
    }

    try {
        roms = loadRomsFromConfig(romsPath);
    } catch(YAML::BadFile& error) {
        spdlog::error("Could not load rom config file, reason: {}", error.msg);
        return EXIT_FAILURE;
    }

    if (roms.empty()) {
        spdlog::error("Config file empty!");
        return EXIT_FAILURE;
    }

    Menu menu(static_cast<int>(roms.size()), 90, 0, 0, roms, emus);
    menu.OnInit();
    menu.OnExecute();
    menu.OnCleanup();
       
    return EXIT_SUCCESS;
}

cxxopts::ParseResult setUpWorkflow(int argc, char** argv, cxxopts::Options & options) {
    int max_depth = 3;

    options.add_options()
        ("i,index", "INDEX Rom directory", cxxopts::value<std::string>())
        ("d,depth", "Index recursion MAX DEPTH, default = 3", cxxopts::value<int>())
        ("v,version", "PRINT program version")
        ("h,help", "PRINT help text")
    ; 

    const auto result = options.parse(argc, argv);

    if (static_cast<bool>(result.count("help"))) {
        fmt::print("{}", options.help());
        exit(EXIT_SUCCESS);
    }

    if (static_cast<bool>(result.count("version"))) {
        printVersion();
        exit(EXIT_SUCCESS);
    }

    if (static_cast<bool>(result.count("depth"))) {
        if (!static_cast<bool>(result.count("index"))) {
            spdlog::warn("Depth option included without accompanying --index option. Ignored.");
        } else {
            max_depth = result["depth"].as<int>();
        }
    }

    if (static_cast<bool>(result.count("index"))) {
        std::vector<Emu> emus{};
        #ifdef RELEASE
            const std::string romsPath = DEFAULT_CONFIG_ROMS_YAML;
            const std::string emuPath =  DEFAULT_CONFIG_EMUS_YAML;
        #elif defined(DEBUG)
            const std::string romsPath = "../config/roms.yaml";
            const std::string emuPath = "../config/emus.yaml";
        #endif

        try {
            emus = loadEmusFromConfig(emuPath);
        } catch(YAML::InvalidNode& error) {
            spdlog::error("Could not parse the emulator config file, reason: {}", error.msg);
            exit(EXIT_FAILURE);
        } catch(YAML::BadFile& error) {
            spdlog::error("Could not load emulator config file, reason: {}", error.msg);
            exit(EXIT_FAILURE);
        }

        std::string dirToIndex = result["index"].as<std::string>();

        std::vector<Rom> roms{};
        fs::path dirPath = expandTilde(dirToIndex);
        try {
            spdlog::debug("Max recursion depth = {}", max_depth);
            index(emus, roms, dirPath, 0, max_depth);
        } catch (fs::filesystem_error err) {
            spdlog::error("Could not index directory '{}', reason: {}", dirPath.string(), err.what());
            exit(EXIT_FAILURE);
        }

        writeRomConfigToFile(roms, romsPath);

        spdlog::info("Finished indexing {}", dirToIndex);
        spdlog::info("Roms found: {}", roms.size());
        exit(EXIT_SUCCESS);
    }

    return result;
}

void printVersion() {
    fmt::print("TinyTerminalRomManager VERSION {}.{}.{}\n", 
        fmt::styled(__VER__MAJOR__, fmt::fg(fmt::color::yellow)),
        fmt::styled(__VER__MINOR__, fmt::fg(fmt::color::yellow)),
        fmt::styled(__VER__PATCH__, fmt::fg(fmt::color::yellow))
    );
}

void printRoms(std::vector<Rom>& roms) {
    for (const auto& rom : roms) {
        fmt::print("{} via {} ({})\n", rom.name, rom.emulator, rom.type);
    }
}

void startEmulator(const Emu & emu, const Rom& rom) {
    pid_t pid = fork();

    if (pid == -1) {
        fmt::print("Fork failed!\n");
        return;
    } 
    
    if (pid == 0) {
        const std::string command = rom.emulator;
        const std::string arg = rom.filename;

        std::vector<char*> args;
        args.push_back(const_cast<char*>(command.c_str()));

        // TODO: This should check that if emu.args.size() > 0, ${FILENAME} ought to be in the emu args array
        if (!emu.args.empty()) {
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
            const std::string fileArg = expandTilde(arg);
            args.push_back(const_cast<char*>(fileArg.c_str()));
            args.push_back(nullptr);
            execvp(args[0], args.data());
        }

        spdlog::error("Error executing command!");
        return;
    }
}