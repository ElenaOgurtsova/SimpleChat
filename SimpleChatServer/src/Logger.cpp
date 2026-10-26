#include <iostream>
#include "Logger.h"

LogLevel g_level = LogLevel::Info;

const char* levelTag(LogLevel level) {
    switch (level) {
    case LogLevel::Error: return "ERROR : ";
    case LogLevel::Warn:  return "WARN : ";
    case LogLevel::Info:  return "INFO : ";
    }
    return "?????";
}

void setLogLevel(LogLevel level) { g_level = level; }
LogLevel logLevel() { return g_level; }

void logMessage(LogLevel level, std::string text) {
    if (static_cast<int>(level) > static_cast<int>(g_level)) return;
    std::cerr << levelTag(level) <<  text << std::endl;
}
