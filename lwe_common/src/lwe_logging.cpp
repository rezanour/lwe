#include <lwe_logging.h>
#include <lwe_assert.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace lwe {

namespace {

lwe::LogLevel g_log_level = lwe::LogLevel::Warning;

char const *GetLogLevelName(LogLevel level) {
  switch (level) {
  case LogLevel::Fatal:   return "FTL";
  case LogLevel::Error:   return "ERR";
  case LogLevel::Warning: return "WRN";
  case LogLevel::Debug:   return "DBG";
  case LogLevel::Info:    return "INF";
  case LogLevel::Verbose: return "VRB";
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

  strcat(message, GetLogLevelName(level));
  strcat(message, ": ");

  size_t const message_length = strlen(message);

  va_list args;
  va_start(args, format);
  vsprintf(message + message_length, format, args);
  va_end(args);

  strcat(message, "\n");

#if LWE_PLATFORM_WINDOWS
  OutputDebugStringA(message);
#elif LWE_PLATFORM_OSX
  fprintf(stderr, "%s", message);
#endif

  LWE_ASSERT(LogLevel::Fatal != level);
}

} // namespace lwe
