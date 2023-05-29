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

    bool searchModeType;
    bool searchModeView;
    std::string searchString;

    void OnRender();
    void MoveUp(int step);
    void MoveDown(int step);
    void ResetMenu();

public:
    Menu(int height, int width, int start_row, int start_col, const std::vector<Rom>& items, const std::vector<Emu>& emus);

    void OnInit();
    void OnExecute();
    void OnCleanup();
};

Emu chooseEmu(std::vector<Emu> emus, std::string path);
void romSearch(const std::vector<Rom>& allRoms, std::vector<Rom>& menu, std::string searchString);

#endif // !WINDOW_H