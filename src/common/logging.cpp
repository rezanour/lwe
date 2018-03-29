#include "logging.h"
#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#endif

static LogLevel g_log_level = LogLevel::Warning;

#ifdef _WIN32
static char const *GetLogLevelName(LogLevel level) {
    switch (level) {
    case LogLevel::Fatal:   return "FATAL";
    case LogLevel::Error:   return "ERROR";
    case LogLevel::Warning: return "WARNING";
    case LogLevel::Debug:   return "DEBUG";
    case LogLevel::Info:    return "INFO";
    case LogLevel::Verbose: return "VERBOSE";
    default: assert(false); return "";
    }
}
#endif

void SetLogLevel(LogLevel log_level) {
    g_log_level = log_level;
}

void LogMessage(LogLevel level, char const *format, ...) {
    if (level > g_log_level) {
        return;
    }

#ifdef _WIN32
    char message[512]{};
    va_list args;
    va_start(args, format);
    vsnprintf_s(message, sizeof(message), format, args);
    va_end(args);
    OutputDebugStringA(GetLogLevelName(level));
    OutputDebugStringA(": ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
#endif
}

