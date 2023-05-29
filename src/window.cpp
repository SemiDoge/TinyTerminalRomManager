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

    this->searchModeType = false;
    this->searchModeView = false;
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
    int ch;
    int sch = '\0';
    Emu emu;
    MEVENT mEvent;

    while (bRunning) {
        OnRender();
        ch = getch();
        switch (ch) {
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
                    if (mEvent.bstate & BUTTON4_PRESSED) {
                        MoveUp(UP_ARROW_STEP);
                    } else if (mEvent.bstate & BUTTON5_PRESSED) {
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
                searchModeType = true;
                scroll_offset  = 0;
                current_option = 0;
                searchString = "";
                sch = '\0';

                while((sch = getch()) != '\n' && searchString.size() < MAX_SEACH_STRING_LEN) {
                    if(sch == KEY_ESCAPE) {
                        searchModeType = false;
                        searchString = "";
                        ResetMenu();
                        OnRender();
                        break;
                    }

                    if(std::isalnum(sch) || std::ispunct(sch) || sch == KEY_BACKSPACE || sch == KEY_SPACE) {
                        sch = tolower(sch);
                        if (sch == KEY_BACKSPACE) {
                            if (searchString.size() > 0 ) {
                                searchString.pop_back();
                            }
                        } else {
                            searchString.push_back(sch);
                        }
                    } else {
                        continue;
                    }

                    if(searchString.size() > 0) {
                        romSearch(roms, menu_items, searchString);
                    } else {
                        ResetMenu();
                    }

                    OnRender();
                }

                OnRender();
                

                break;
            case KEY_ESCAPE:
                if(searchModeType) {
                    searchModeType = false;
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

    if(searchModeType) {
        mvprintw(0, 0, "Search: %s (%ld results)\tPress ENTER to enter select mode. Press ESC to return to full listing.", searchString.c_str(), menu_items.size());
    } else {
        #ifdef RELEASE
        mvprintw(0, 0, "ROMs: %ld\tSelected: %s", menu_items.size(), menu_items[current_option].name.c_str());
        #elif defined(DEBUG)
        mvprintw(0, 0, "ROMS: %ld SEL: %d(%d) MENU_HEIGHT: %d MAXYLINES: %d", menu_items.size(), current_option, scroll_offset, menu_height, maxyLines);
        #endif
    }

    for (int i = 0; i < menu_height; i++) {
        int index = i + scroll_offset;
        if (index  == current_option) {
            attron(A_REVERSE);
        }

        //simplify this, check line 85
        if (index < std::min((int) menu_items.size(), maxyLines+scroll_offset)) {
            int lastSlash = menu_items[index].emulator.find_last_of("/\\");
            std::string emuString = menu_items[index].emulator.substr(lastSlash + 1, menu_items[index].emulator.size());

            std::string romString = fmt::format("[{}] {} via {}", 
                    menu_items[index].type, menu_items[index].name, 
                    emuString
                    
            ).c_str();
            mvwprintw(stdscr, i + NUM_NON_ENTRY_LINES, 0, "%s", romString.c_str());
        } else {
            wrefresh(stdscr);
            attroff(A_REVERSE);
            break;
        }

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

    if(current_option - step >= 0) {
        if(scroll_offset > 0 && (current_option - step) - scroll_offset < 0) {
            scroll_offset -= step;
        }
        current_option -= step;
    }
}

void Menu::MoveDown(int step) {
    int maxyLines = getmaxy(stdscr) - NUM_NON_ENTRY_LINES;
    int menuItemSize = (int) menu_items.size();

    if(current_option + step >= menuItemSize) {
        current_option = menuItemSize - 1;
        scroll_offset = std::max(0, current_option - (maxyLines - 1));
        return;
    }
    
    if (current_option + step < (maxyLines) + scroll_offset && current_option + step < menuItemSize) {
        current_option += step;
    } else {
        scroll_offset += step;
        current_option += step;
    } 
}


void Menu::ResetMenu() {
    this->menu_items.clear();
    this->menu_items.reserve(roms.size());
    std::copy(roms.begin(), roms.end(), std::back_inserter(this->menu_items));
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

