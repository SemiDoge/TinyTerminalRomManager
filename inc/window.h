#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <string>
#include <iostream>
#include <ncurses.h>
#include "../inc/main.h"

#define NUM_NON_ENTRY_LINES 1
#define KEY_COMBO_CTRL_U  21
#define KEY_COMBO_CTRL_D  4
#define CTRL_U_STEP       15
#define CTRL_D_STEP       15
#define UP_ARROW_STEP     1
#define DOWN_ARROW_STEP   1

class Menu {
private:
    int menu_height;
    int menu_width;
    int start_row;
    int start_col;
    int scroll_offset;
    std::vector<Rom> menu_items;
    bool bRunning;

    void OnRender();
    void ScrollUp(int step);
    void ScrollDown(int step);

public:
    Menu(int height, int width, int start_row, int start_col, const std::vector<Rom>& items);
    void OnInit();
    void OnExecute();
    void OnCleanup();
};

#endif