#include "../inc/window.h"

Menu::Menu(int height, int width, int start_row, int start_col,
    const std::vector<Rom>& items, const std::vector<Emu>& emus) 
{
    this->menu_height = height;
    this->menu_width = width;
    this->start_row = start_row;
    this->start_col = start_col;
    this->scroll_offset = 0;
    this->menu_items = items;
    this->emus = emus;
    this->bRunning = true;
}

void Menu::OnInit() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // init mouse scroll wheels
    mmask_t mask = BUTTON4_PRESSED | BUTTON5_PRESSED;
    mousemask(mask, NULL);
}

void Menu::OnExecute() {
    int ch;
    Emu emu;
    MEVENT mEvent;
    while (bRunning) {
        OnRender();
        ch = getch();
        switch (ch) {
            case KEY_UP:
                ScrollUp(UP_ARROW_STEP);
                break;
            case KEY_DOWN:
                ScrollDown(DOWN_ARROW_STEP);
                break;
            case KEY_COMBO_CTRL_U:
                ScrollUp(CTRL_U_STEP);
                break;
            case KEY_COMBO_CTRL_D:
                ScrollDown(CTRL_D_STEP);
                break;
            case KEY_MOUSE:
                if (getmouse(&mEvent) == OK) {
                    if (mEvent.bstate & BUTTON4_PRESSED) {
                        ScrollUp(UP_ARROW_STEP);
                        break;
                    } else if (mEvent.bstate & BUTTON5_PRESSED) {
                        ScrollDown(DOWN_ARROW_STEP);
                        break;
                    }
                }
                break;
            case '\n':
                emu = chooseEmu(emus, menu_items[scroll_offset].emulator);
                startEmulator(emu, menu_items[scroll_offset]);
                bRunning = false;
                break;
            case '/':
                //Search goes here
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
    int maxyLines = getmaxy(stdscr);
    mvprintw(0, 0, "ROMS: %ld  SEL: %d", menu_items.size(), scroll_offset);
    for (int i = 0; i < menu_height; i++) {
        int index = i + scroll_offset;
        if (index  == scroll_offset) {
            attron(A_REVERSE);
        }

        if (index < std::min((int) menu_items.size(), maxyLines+scroll_offset)) {
            int lastSlash = menu_items[index].emulator.find_last_of("/\\");
            std::string emuString = menu_items[index].emulator.substr(lastSlash + 1, menu_items[index].emulator.size());

            std::string romString = fmt::format("[{}] {} via {}", 
                    menu_items[index].type, menu_items[index].name, 
                    emuString
                    
            ).c_str();
            mvwprintw(stdscr, i + 1, 0, "%s", romString.c_str());
        } else {
            wrefresh(stdscr);
            attroff(A_REVERSE);
            break;
        }

        wrefresh(stdscr);
        attroff(A_REVERSE);
    }
}

void Menu::ScrollUp(int step) {
    if (scroll_offset - step < 0) {
        scroll_offset = 0;
        return;
    } else {
        scroll_offset -= step;
    }
}

void Menu::ScrollDown(int step) {
    if (scroll_offset + step > (int) menu_items.size() - 2) {
        scroll_offset = (int) menu_items.size() - 1;
    } else {
        scroll_offset += step;
    }
}