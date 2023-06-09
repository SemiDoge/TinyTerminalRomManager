#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fmt/format.h>

std::string toUpper(std::string str);
std::string toLower(std::string str);
std::string extractExtension(const std::string& path);
std::string expandTilde(const std::string& path);

#ifdef _WIN32
#include <shlobj.h>
std::string WideStringToString(const std::wstring& wideString);
#endif // _WIN32

#endif // !UTILS_H