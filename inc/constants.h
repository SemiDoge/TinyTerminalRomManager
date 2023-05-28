#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "config.h"

#define DEBUG
#define NUM_NON_ENTRY_LINES 1
#define KEY_COMBO_CTRL_U    21
#define KEY_COMBO_CTRL_D    4  
#define CTRL_U_STEP         15
#define CTRL_D_STEP         15
#define UP_ARROW_STEP       1
#define DOWN_ARROW_STEP     1

constexpr float __VER__MAJOR__ = 0;
constexpr float __VER__MINOR__ = 2;
constexpr float __VER__PATCH__ = 2;

const std::string DEFAULT_CONFIG_ROMS_YAML = expandTilde("~/.config/romManager/roms.yaml");
const std::string DEFAULT_CONFIG_EMUS_YAML = expandTilde("~/.config/romManager/emus.yaml");

#endif // !CONSTANTS_H