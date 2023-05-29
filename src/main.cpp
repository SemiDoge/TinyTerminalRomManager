#include "../inc/main.h"

cxxopts::ParseResult setUpWorkflow(int argc, char** argv, cxxopts::Options & options);
void printVersion();


//TODO: Set up testing
//TODO: Save file management? After above stuff is finished
//TODO: Idea: for save file management maybe I can let saves be saved to the RomManager dir and then copy to desired folder
int main(int argc, char** argv) {
    spdlog::set_pattern("%^%l%$: %v");
    cxxopts::Options options("romManager", 
        "A simple ncurses driven ROM manager, for all your emulation management needs!\nPlease use this tool with only legally obtained ROMS."
    );

    try {
        const auto optRes = setUpWorkflow(argc, argv, options);
    } catch (cxxopts::exceptions::missing_argument error) {
        spdlog::error("{}", error.what());
        return -1;
    }

    #ifdef RELEASE
        const std::string romsPath = DEFAULT_CONFIG_ROMS_YAML;
        const std::string emuPath =  DEFAULT_CONFIG_EMUS_YAML;
    #elif defined(DEBUG)
        const std::string romsPath = "../config/test.yaml";
        const std::string emuPath = "../config/emus.yaml";
    #endif

    std::vector<Rom> roms{};
    std::vector<Emu> emus{};

    try {
        emus = loadEmusFromConfig(emuPath);
    } catch(YAML::InvalidNode& error) {
        spdlog::error("Could not parse the emulator config file, reason: {}", error.msg);
        return -1;
    } catch(YAML::BadFile& error) {
        spdlog::error("Could not load emulator config file, reason: {}", error.msg);
        return -1;
    }

    try {
        roms = loadRomsFromConfig(romsPath);
    } catch(YAML::BadFile& error) {
        spdlog::error("Could not load rom config file, reason: {}", error.msg);
        return -1;
    }

    if (roms.size() == 0) {
        spdlog::error("Config file empty!");
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
        ("i,index", "INDEX Rom directory", cxxopts::value<std::string>())
        ("v,version", "PRINT program version")
        ("h,help", "PRINT help text")
    ; 

    const auto result = options.parse(argc, argv);

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
            const std::string romsPath = DEFAULT_CONFIG_ROMS_YAML;
            const std::string emuPath =  DEFAULT_CONFIG_EMUS_YAML;
        #elif defined(DEBUG)
            const std::string romsPath = "../config/test.yaml";
            const std::string emuPath = "../config/emus.yaml";
        #endif

        try {
            emus = loadEmusFromConfig(emuPath);
        } catch(YAML::InvalidNode& error) {
            spdlog::error("Could not parse the emulator config file, reason: {}", error.msg);
            exit(-1);
        } catch(YAML::BadFile& error) {
            spdlog::error("Could not load emulator config file, reason: {}", error.msg);
            exit(-1);
        }

        std::string dirToIndex = result["index"].as<std::string>();

        spdlog::info("Indexing: {}", dirToIndex);
        auto ret = index(emus, dirToIndex);

        spdlog::info("Finished indexing {}", dirToIndex);
        spdlog::info("Roms found: {}", ret.size());
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
        const std::string command = rom.emulator;
        const std::string arg = rom.filename;

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
            const std::string fileArg = expandTilde(arg);
            args.push_back(const_cast<char*>(fileArg.c_str()));
            args.push_back(nullptr);
            execvp(args[0], args.data());
        }

        fmt::print("Error executing command!\n");
        return;
    }
}