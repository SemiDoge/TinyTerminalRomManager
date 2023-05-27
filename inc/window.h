#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <string>
#include <iostream>
#include <ncurses.h>
#include <fmt/format.h>

#include "../inc/main.h"
#include "../inc/constants.h"
#include "../inc/structs.h"

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