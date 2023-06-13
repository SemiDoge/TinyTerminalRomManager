#include "../inc/window.h"

Menu::Menu(int height, int width, int start_row, int start_col,
    const std::vector<Rom>& items, const std::vector<Emu>& emus) {
    this->menu_height = height;
    this->menu_width = width;
    this->start_row = start_row;
    this->start_col = start_col;
    this->scroll_offset = 0;
    this->current_option = 0;
    this->roms = items;
    this->emus = emus;
    this->bRunning = true;

    this->searchMode = false;
    this->searchString = "";

    ResetMenu();
}

#ifdef _WIN32
    void Menu::OnInit(HANDLE stdout_hdl) {
        this->stdout_hdl = stdout_hdl;
        CONSOLE_CURSOR_INFO cursorInfo;

        // Hide the cursor
        GetConsoleCursorInfo(stdout_hdl, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(stdout_hdl, &cursorInfo);
    }

    void Menu::OnExecute() {
        int chr;
        Emu emu;

        OnRender();

        while (bRunning) {
            chr = _getch();
            switch (chr) {
                case KEY_UP:
                    MoveUp(UP_ARROW_STEP);
                    break;
                case KEY_DOWN:
                    MoveDown(DOWN_ARROW_STEP);
                    break;
                case KEY_COMBO_CTRL_U:
                    MoveUp(CTRL_U_STEP);
                    break;
                case KEY_COMBO_CTRL_D:
                    MoveDown(CTRL_D_STEP);
                    break;
                case KEY_ENTER:
                    emu = chooseEmu(emus, menu_items[current_option].emulator);
                    startEmulator(emu, menu_items[current_option]);
                    bRunning = false;
                    break;
                case KEY_COMBO_CTRL_F:
                case '/':
                    SearchDriver();
                    OnRender();
                    break;
                case KEY_ESCAPE:
                    if(searchMode) {
                        searchMode = false;
                        scroll_offset = 0;
                        current_option = 0;
                        ResetMenu();
                    }
                    break;
                case 'Q':
                case 'q': 
                    bRunning = false;
                    break;
                default:
                    continue;
            }

            OnRender();
        }
    }

    void Menu::OnCleanup() {
        system("cls");
    }

    void Menu::OnRender() {
        system("cls");

        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        GetConsoleScreenBufferInfo(stdout_hdl, &bufferInfo);
        int maxyLines = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;
        // int maxyLines = TTRM_GETMAXY(stdout_hdl) + 1;

        {
            COORD pos = {0, 0};
            SetConsoleCursorPosition(stdout_hdl, pos);
            if(searchMode) {
                fmt::print("Search: {} ({} results)\tPress ENTER to enter select mode. Press ESC to return to full listing.\n", searchString, menu_items.size());
            } else {
                #ifdef RELEASE
                fmt::print("ROMs: {}\tSelected: {}", menu_items.size(), menu_items[current_option].name);
                #elif defined(DEBUG)
                fmt::print("ROMS: {} SEL: {}({}) MAXYLINES: {}\n", menu_items.size(), current_option, scroll_offset, maxyLines);
                #endif
            }
        }

        // The -2 in (maxyLines - 2) prevents the terminal scroll bar from appearing and messing up the placement of the rom strings
        int endIndex = std::min(static_cast<int>(menu_items.size()), (maxyLines - 2) + scroll_offset);
        for (int i = scroll_offset; i < endIndex; i++) {
            if (i == current_option) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | BACKGROUND_INTENSITY); // Highlight selected option
            }

            int lastSlash = static_cast<int>(menu_items[i].emulator.find_last_of("/\\"));
            std::string emuString = menu_items[i].emulator.substr(lastSlash + 1);
            std::string romString = fmt::format("[{:^5}] {} via {}", menu_items[i].type, menu_items[i].name, emuString);

            COORD pos = {0, static_cast<short>(i - scroll_offset + NUM_NON_ENTRY_LINES)};
            SetConsoleCursorPosition(stdout_hdl, pos);
            fmt::print("{}\n", romString);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); // Reset color
        }
    }

    void Menu::SearchDriver() {
        int sch = '\0';
        searchMode = true;
        scroll_offset  = 0;
        current_option = 0;
        searchString = "";

        while((sch = _getch()) != KEY_ENTER) {
            if(sch == KEY_ESCAPE) {
                searchMode = false;
                scroll_offset  = 0;
                current_option = 0;
                searchString = "";

                ResetMenu();
                break;
            }

            if((std::isalnum(sch)) != 0 || (std::ispunct(sch)) != 0 || sch == KEY_BACKSPACE || sch == KEY_SPACE) {
                sch = tolower(sch);
                if (sch == KEY_BACKSPACE && !searchString.empty()) {
                    searchString.pop_back();
                } else if (searchString.size() < MAX_SEACH_STRING_LEN && sch != KEY_BACKSPACE) {
                    searchString.push_back(static_cast<char>(sch));
                }
            }

            if(!searchString.empty()) {
                Search(searchString);
            } else {
                ResetMenu();
            }

            OnRender();
        }
    }

    #elif __linux__
    void Menu::OnInit() {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);

        // init mouse scroll wheels
        const mmask_t mask = BUTTON4_PRESSED | BUTTON5_PRESSED;
        mousemask(mask, NULL);
    }

    void Menu::OnExecute() {
        int chr;
        Emu emu;
        MEVENT mEvent;

        while (bRunning) {
            OnRender();
            chr = getch();
            switch (chr) {
                case KEY_UP:
                    MoveUp(UP_ARROW_STEP);
                    break;
                case KEY_DOWN:
                    MoveDown(DOWN_ARROW_STEP);
                    break;
                case KEY_COMBO_CTRL_U:
                    MoveUp(CTRL_U_STEP);
                    break;
                case KEY_COMBO_CTRL_D:
                    MoveDown(CTRL_D_STEP);
                    break;
                case KEY_MOUSE:
                    if (getmouse(&mEvent) == OK) {
                        if ((mEvent.bstate & BUTTON4_PRESSED) != 0) {
                            MoveUp(UP_ARROW_STEP);
                        } else if ((mEvent.bstate & BUTTON5_PRESSED) != 0) {
                            MoveDown(DOWN_ARROW_STEP);
                        }
                    }
                    break;
                case '\n':
                    emu = chooseEmu(emus, menu_items[current_option].emulator);
                    startEmulator(emu, menu_items[current_option]);
                    bRunning = false;
                    break;
                case KEY_COMBO_CTRL_F:
                case '/':
                    SearchDriver();
                    OnRender();
                    break;
                case KEY_ESCAPE:
                    if(searchMode) {
                        searchMode = false;
                        scroll_offset = 0;
                        current_option = 0;
                        ResetMenu();
                    }
                    break;
                case 'Q':
                case 'q': 
                    bRunning = false;
                    break;

            }
        }
    }

    void Menu::OnCleanup() {
        endwin();
    }

    void Menu::OnRender() {
        wclear(stdscr);
        // int maxyLines = getmaxy(stdscr) - NUM_NON_ENTRY_LINES;
        int maxyLines = TTRM_GETMAXY(stdscr) - NUM_NON_ENTRY_LINES;

        if(searchMode) {
            mvprintw(0, 0, "Search: %s (%ld results)\tPress ENTER to enter select mode. Press ESC to return to full listing.", searchString.c_str(), menu_items.size());
        } else {
            #ifdef RELEASE
            mvprintw(0, 0, "ROMs: %ld\tSelected: %s", menu_items.size(), menu_items[current_option].name.c_str());
            #elif defined(DEBUG)
            mvprintw(0, 0, "ROMS: %ld SEL: %d(%d) MENU_HEIGHT: %d MAXYLINES: %d", menu_items.size(), current_option, scroll_offset, menu_height, maxyLines);
            #endif
        }

        int endIndex = std::min(static_cast<int>(menu_items.size()), maxyLines + scroll_offset);
        for (int i = scroll_offset; i < endIndex; i++) {
            if (i == current_option) {
                attron(A_REVERSE);
            }

            int lastSlash = static_cast<int>(menu_items[i].emulator.find_last_of("/\\"));
            std::string emuString = menu_items[i].emulator.substr(lastSlash + 1);
            std::string romString = fmt::format("[{:^5}] {} via {}", menu_items[i].type, menu_items[i].name, emuString);

            mvwprintw(stdscr, i - scroll_offset + NUM_NON_ENTRY_LINES, 0, "%s", romString.c_str());
            wrefresh(stdscr);
            attroff(A_REVERSE);
        }
    }

    void Menu::SearchDriver() {
        int sch = '\0';
        searchMode = true;
        scroll_offset  = 0;
        current_option = 0;
        searchString = "";

        while((sch = getch()) != '\n') {
            if(sch == KEY_ESCAPE) {
                searchMode = false;
                scroll_offset  = 0;
                current_option = 0;
                searchString = "";

                ResetMenu();
                break;
            }

            if((std::isalnum(sch)) != 0 || (std::ispunct(sch)) != 0 || sch == KEY_BACKSPACE || sch == KEY_SPACE) {
                sch = tolower(sch);
                if (sch == KEY_BACKSPACE && !searchString.empty()) {
                    searchString.pop_back();
                } else if (searchString.size() < MAX_SEACH_STRING_LEN && sch != KEY_BACKSPACE) {
                    searchString.push_back(static_cast<char>(sch));
                }
            }

            if(!searchString.empty()) {
                Search(searchString);
            } else {
                ResetMenu();
            }

            OnRender();
        }
    }
#endif // _WIN32

void Menu::MoveUp(int step) {
    if (current_option - step < 0) {
        scroll_offset = 0;
        current_option = 0;
        return;
    }

    if (scroll_offset - step < 0) {
        scroll_offset = 0;
    }

    if (scroll_offset > 0 && current_option - step - scroll_offset < 0) {
        scroll_offset -= step;
    }
    
    current_option -= step;
}

void Menu::MoveDown(int step) {
    #ifdef _WIN32
    int maxVisibleLines = TTRM_GETMAXY(stdout_hdl) - NUM_NON_ENTRY_LINES;
    #elif __linux__
    int maxVisibleLines = TTRM_GETMAXY(stdscr) - NUM_NON_ENTRY_LINES;
    #endif // _WIN32

    int totalMenuItems = static_cast<int>(menu_items.size());

    if (current_option + step >= totalMenuItems) {
        current_option = totalMenuItems - 1;
        scroll_offset = std::max(0, current_option - (maxVisibleLines - 1));
        return;
    }
    
    if (current_option + step < maxVisibleLines + scroll_offset && current_option + step < totalMenuItems) {
        current_option += step;
    } else {
        scroll_offset += step;
        current_option += step;
    } 
}

void Menu::ResetMenu() {
    menu_items.clear();
    menu_items.reserve(roms.size());
    std::copy(roms.begin(), roms.end(), std::back_inserter(menu_items));
}

void Menu::Search(const std::string& searchString) {
    std::vector<Rom> searchResult = fuzzySearch(roms, searchString, 83);
    menu_items.clear();
    menu_items.reserve(searchResult.size());
    std::copy(searchResult.begin(), searchResult.end(), std::back_inserter(menu_items));
}

Emu chooseEmu(std::vector<Emu> emus, const std::string& path) {
    for(const auto& emu: emus) {
        if(emu.filename == path) {
            return emu;
        }
    }

    return emus[0];
}


#ifdef _WIN32
int getMaxY(HANDLE stdout_hdl) {
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(stdout_hdl, &bufferInfo);
    return (int) bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top;
}
#endif // _WIN32
