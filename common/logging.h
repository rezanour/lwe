#pragma once

#include <assert.h>

enum class LogLevel {
    Fatal = 0,
    Error,
    Warning,
    Debug,
    Info,
    Verbose,
};

void SetLogLevel(LogLevel log_level);
void LogMessage(LogLevel level, char const *format, ...);

#define LWE_FATAL(format, ...)   LogMessage(LogLevel::Fatal, format, __VA_ARGS__);
#define LWE_ERROR(format, ...)   LogMessage(LogLevel::Error, format, __VA_ARGS__);
#define LWE_WARN(format, ...)    LogMessage(LogLevel::Warning, format, __VA_ARGS__);
#define LWE_DEBUG(format, ...)   LogMessage(LogLevel::Debug, format, __VA_ARGS__);
#define LWE_INFO(format, ...)    LogMessage(LogLevel::Info, format, __VA_ARGS__);
#define LWE_VERBOSE(format, ...) LogMessage(LogLevel::Verbose, format, __VA_ARGS__);
