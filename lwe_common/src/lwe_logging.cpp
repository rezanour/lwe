#include <lwe_logging.h>
#include <lwe_assert.h>

#include <stdio.h>
#include <stdarg.h>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace lwe {

namespace {

lwe::LogLevel g_log_level = lwe::LogLevel::Warning;

char const *GetLogLevelName(LogLevel level) {
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

} // namespace

void SetLogLevel(LogLevel log_level) {
  g_log_level = log_level;
}

void LogMessage(LogLevel level, char const *format, ...) {
  if (level > g_log_level) {
    return;
  }

  char message[512]{};

  strcat_s(message, GetLogLevelName(level));
  strcat_s(message, ": ");

  size_t const message_length = strlen(message);

  va_list args;
  va_start(args, format);
  vsprintf_s(message + message_length, sizeof(message) - message_length, format, args);
  va_end(args);

  strcat_s(message, "\n");

#if LWE_PLATFORM_WINDOWS
  OutputDebugStringA(message);
#endif

  LWE_ASSERT(LogLevel::Fatal != level);
}

} // namespace lwe
