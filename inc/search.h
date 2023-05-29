#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <string>
#include <map>

#include "structs.h"
#include "config.h"

int levenshteinDistance(const std::string& str1, const std::string& str2);
std::vector<Rom> fuzzySearch(const std::vector<Rom>& roms, const std::string& searchString, int maxDistance);
void romSearch(const std::vector<Rom>& allRoms, std::vector<Rom>& menu, std::string searchString);

#endif // !SEARCH_H
