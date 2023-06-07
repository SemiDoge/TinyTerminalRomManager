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
    int maxyLines = getmaxy(stdscr) - NUM_NON_ENTRY_LINES;

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

void Menu::MoveUp(int step) {
    if (current_option - step < 0) {
        current_option = 0;
        scroll_offset = 0;
        return;
    }

    if (scroll_offset > 0 && current_option - step - scroll_offset < 0) {
        scroll_offset -= step;
    }
    
    current_option -= step;
}

void Menu::MoveDown(int step) {
    int maxVisibleLines = getmaxy(stdscr) - NUM_NON_ENTRY_LINES;
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