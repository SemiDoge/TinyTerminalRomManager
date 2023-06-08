#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "utils.h"

#define DEBUG
#define NUM_NON_ENTRY_LINES     1
#define KEY_ESCAPE              27
#define KEY_SPACE               32
#define KEY_COMBO_CTRL_U        ('u' & 0x1F)
#define KEY_COMBO_CTRL_D        ('d' & 0x1F)
#define KEY_COMBO_CTRL_F        ('f' & 0x1F)
#define CTRL_U_STEP             15
#define CTRL_D_STEP             15
#define UP_ARROW_STEP           1
#define DOWN_ARROW_STEP         1
#define MAX_SEACH_STRING_LEN    30

#ifdef _WIN32
#define KEY_UP           72
#define KEY_DOWN         80
#define KEY_ENTER        13
#define KEY_BACKSPACE    8
#endif // _WIN32


constexpr float __VER__MAJOR__ = 0;
constexpr float __VER__MINOR__ = 6;
constexpr float __VER__PATCH__ = 0;

const std::string DEFAULT_CONFIG_ROMS_YAML = expandTilde("~/.config/TinyTermRomManager/roms.yaml");
const std::string DEFAULT_CONFIG_EMUS_YAML = expandTilde("~/.config/TinyTermRomManager/emus.yaml");

#endif // !CONSTANTS_H