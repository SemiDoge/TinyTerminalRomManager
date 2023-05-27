#include "../inc/window.h"

Menu::Menu(int height, int width, int start_row, int start_col, const std::vector<Rom>& items) {
    this->menu_height = height;
    this->menu_width = width;
    this->start_row = start_row;
    this->start_col = start_col;
    this->scroll_offset = 0;
    this->menu_items = items;
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
                startEmulator(menu_items[scroll_offset]);
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
            attron(A_REVERSE); // Highlight the currently selected option
        }

        if (index < std::min((int) menu_items.size(), maxyLines+scroll_offset)) {
            std::string romString = fmt::format("[{}] {} via {}", 
                    menu_items[index].type, menu_items[index].name, 
                    menu_items[index].emulator
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