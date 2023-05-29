#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "config.h"

#define DEBUG
#define NUM_NON_ENTRY_LINES  1
#define KEY_COMBO_CTRL_U     ('u' & 0x1f)
#define KEY_COMBO_CTRL_D     ('d' & 0x1f)
#define KEY_COMBO_CTRL_F     ('f' & 0x1f)
#define CTRL_U_STEP          15
#define CTRL_D_STEP          15
#define UP_ARROW_STEP        1
#define DOWN_ARROW_STEP      1

constexpr float __VER__MAJOR__ = 0;
constexpr float __VER__MINOR__ = 3;
constexpr float __VER__PATCH__ = 1;

const std::string DEFAULT_CONFIG_ROMS_YAML = expandTilde("~/.config/romManager/roms.yaml");
const std::string DEFAULT_CONFIG_EMUS_YAML = expandTilde("~/.config/romManager/emus.yaml");

#endif // !CONSTANTS_H