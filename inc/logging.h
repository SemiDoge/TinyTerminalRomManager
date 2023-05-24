#ifndef LOGGING_H
#define LOGGING_H


#include <fmt/format.h>
#include <fmt/color.h>

enum logSeverity {
    ERROR = -1,
    INFO,
    TRACE,
    DEBUG,
};

class Logger {
private:
    static inline bool bAllowedToPrint;

public:
    static void log(std::string msg, logSeverity sever);
    static void setAllowedToPrint(bool bAllowedToPrint);
};


#endif