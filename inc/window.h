#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <string>
#include <iostream>
#include <ncurses.h>
#include <algorithm>
#include <fmt/format.h>

#include "../inc/main.h"
#include "../inc/constants.h"
#include "../inc/structs.h"
#include "../inc/search.h"

class Menu {
private:
    int menu_height;
    int menu_width;
    int start_row;
    int start_col;
    int scroll_offset;
    int current_option;
    std::vector<Rom> roms;
    std::vector<Rom> menu_items;
    std::vector<Emu> emus;
    bool bRunning;

    bool searchMode;
    std::string searchString;

    void OnRender();
    void MoveUp(int step);
    void MoveDown(int step);
    void SearchDriver();
    void Search(const std::string& searchString);
    void ResetMenu();

public:
    Menu(int height, int width, int start_row, int start_col, const std::vector<Rom>& items, const std::vector<Emu>& emus);

    void OnInit();
    void OnExecute();
    void OnCleanup();
};

Emu chooseEmu(std::vector<Emu> emus, const std::string& path);

#endif // !WINDOW_H