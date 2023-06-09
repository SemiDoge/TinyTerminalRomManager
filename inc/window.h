#ifndef WINDOW_H
#define WINDOW_H

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#undef min
#undef max
#elif __linux__
#include <ncurses.h>
#endif // _WIN32

#include <vector>
#include <string>
#include <iostream>
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

    #ifdef _WIN32
    HANDLE stdout_hdl; 
    #endif // _WIN32

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

    #ifdef _WIN32
    void OnInit(HANDLE stdout_hdl);
    #elif __linux__
    void OnInit();
    #endif // _WIN32

    void OnExecute();
    void OnCleanup();
};

Emu chooseEmu(std::vector<Emu> emus, const std::string& path);

#endif // !WINDOW_H