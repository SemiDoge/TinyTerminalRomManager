#include "../inc/main.h"
#include "../inc/logging.h"
#include "../inc/config.h"
#include "../inc/window.h"

bool bExitFlag = false;

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

    ctx context;
    context.roms = roms;

    initWindow();

    int currentOption = 0;
    int numOptions = context.roms.size();

    // Main menu loop
    while (true) {
        clear();

        mvprintw(0, 1, "ROMS");

        // Display menu options
        for (int i = 0; i < numOptions; ++i) {
            if (i == currentOption) {
                attron(A_REVERSE); // Highlight the currently selected option
            }

            std::string romString = fmt::format("[{}] {} via {}", context.roms[i].type, context.roms[i].name, context.roms[i].emulator).c_str();
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
            case 'q' | 'Q':
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
