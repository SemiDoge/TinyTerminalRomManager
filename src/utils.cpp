#include "../inc/utils.h"

std::string extractExtension(const std::string& path) {
    size_t lastDot = path.find_last_of('.');

    return path.substr(lastDot + 1, path.length());
}

std::string toUpper(std::string str) {
    for (auto & chr: str) chr = static_cast<unsigned char>(toupper(chr));
    return str;
}

std::string toLower(std::string str) {
    for (auto & chr: str) chr =  static_cast<unsigned char>(tolower(chr));
    return str;
}

std::string expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    std::string expandedPath;
    const char* homeDir = std::getenv("HOME");

    if (homeDir != nullptr) {
        expandedPath = fmt::format("{}{}", homeDir, path.substr(1));
    } else {
        expandedPath = fmt::format("/home/username{}", homeDir, path.substr(1));
    }

    return expandedPath;
}