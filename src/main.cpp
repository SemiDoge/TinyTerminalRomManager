#include "../inc/main.h"
#include "../inc/logging.h"
#include "../inc/config.h"

bool bExitFlag = false;
#define DEBUG


// int main() {
//     // std::string path = "../config/emus.yaml";
//     std::vector<Rom> roms{};

//     writeDirToRomConfig(roms, expandTilde("~/RomManager/"));
//     writeRomConfigToFile(roms, "../config/test.yaml");

// }

//TODO: Need to implement scanner invokable via cmd, consider adding CXXOPTS dependency
//TODO: Search/Filtering option
int main() {
    #ifdef RELEASE
    std::string path = expandTilde("~/.config/romManager/roms.yaml");
    #elif defined(DEBUG)
    // std::string path = "../config/roms-original.yaml";
    std::string path = "../config/test.yaml";
    #endif
    Logger::setAllowedToPrint(true);

    std::vector<Rom> roms{};
    std::vector<Emu> emus{};

    try {
        roms = loadRomsFromConfig(path);
    }
    catch(YAML::BadFile& error) {
        Logger::log(fmt::format("Could not load rom config file, reason: {}", error.msg), logSeverity::ERROR);
        return -1;
    }

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

    initWindow();

    int currentOption = 0;
    int numOptions = roms.size();
    // TODO: Make a slide-able 'view' that responds to the max viewable size of the window.
    int visibleNumLines, numCols;
    (void) numCols;

    if (numOptions == 0) {
        Logger::log("Config file empty!\n", logSeverity::ERROR);
        return -1;
    }

    // Main menu loop
    while (true) {
        clear();

        getmaxyx(stdscr, visibleNumLines, numCols);
        mvprintw(0, 1, "ROMS: %d", numOptions);

        // Display menu options
        for (int i = 0; i < numOptions; ++i) {
            if (i == currentOption) {
                attron(A_REVERSE); // Highlight the currently selected option
            }

            std::string romString = fmt::format("[{}] {} via {}", roms[i].type, roms[i].name, roms[i].emulator).c_str();
            mvprintw(i + 1, 1, "%s", romString.c_str());
            attroff(A_REVERSE);
        }


        // Get user input
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                currentOption = (currentOption - 1 + numOptions) % numOptions;
                break;
            case KEY_DOWN:
                currentOption = (currentOption + 1) % numOptions;
                break;
            case '\n':
                startEmulator(roms[currentOption]);
                bExitFlag = true;
                break;
            case KEY_COMBO_CTRL_U:
                currentOption = (currentOption - CTRL_D_STEP + numOptions) % numOptions;
                break;
            case KEY_COMBO_CTRL_D:
                currentOption = (currentOption + CTRL_D_STEP + numOptions) % numOptions;
                break;
            case 'Q':
            case 'q':
                bExitFlag = true;
                break;

        }

        if (bExitFlag == true) {
            break;
        }
    }

    // Cleanup and exit
    endwin();
       
    return 0;
}

void initWindow() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    return;
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
