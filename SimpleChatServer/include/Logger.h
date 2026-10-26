#pragma once

#include <iostream>

enum class LogLevel { Error = 0, Warn = 1, Info = 2};

void setLogLevel(LogLevel level);
LogLevel logLevel();

void logMessage(LogLevel level, std::string text);

inline void logError(std::string t) { logMessage(LogLevel::Error, t);}
inline void logWarn(std::string t)  { logMessage(LogLevel::Warn, t);}
inline void logInfo(std::string t)  { logMessage(LogLevel::Info, t);}
