#ifndef CONFIG_H
#define CONFIG_H

#include <assert.h>
#include <yaml-cpp/yaml.h>
#include "main.h"

void writeRomToConfig(Rom rom);
std::vector<Rom> loadRomsFromConfig(std::string fileName);

#endif