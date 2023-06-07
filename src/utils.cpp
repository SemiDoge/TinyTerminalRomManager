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

#ifdef _WIN32
std::string expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    std::string expandedPath;

    PWSTR wpath;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &wpath);
    CoTaskMemFree(static_cast<void*>(wpath));

    std::wstring widePath(wpath);
    std::string homeDir = WideStringToString(widePath);

    if (!homeDir.empty()) {
        expandedPath = fmt::format("{}{}", homeDir, path.substr(1));
    } else {
        expandedPath = path;
    }

    return expandedPath;
}

std::string WideStringToString(const std::wstring& wideString)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (bufferSize == 0)
    {
        return "";
    }

    std::string result(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &result[0], bufferSize, nullptr, nullptr);
    result.resize(bufferSize - 1);  // Exclude the null-terminating character
    return result;
}
#elif __linux__
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
#endif // _WIN32