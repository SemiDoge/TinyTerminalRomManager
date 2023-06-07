#ifndef UTILS_H

#include <string>
#include <fmt/format.h>

std::string toUpper(std::string str);
std::string toLower(std::string str);
std::string extractExtension(const std::string& path);
std::string expandTilde(const std::string& path);

#endif // !UTILS_H