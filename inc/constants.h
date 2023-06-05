#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "config.h"

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

constexpr float __VER__MAJOR__ = 0;
constexpr float __VER__MINOR__ = 4;
constexpr float __VER__PATCH__ = 4;

const std::string DEFAULT_CONFIG_ROMS_YAML = expandTilde("~/.config/TinyTermRomManager/roms.yaml");
const std::string DEFAULT_CONFIG_EMUS_YAML = expandTilde("~/.config/TinyTermRomManager/emus.yaml");

#endif // !CONSTANTS_H