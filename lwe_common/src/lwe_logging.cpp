#include <lwe_logging.h>
#include <lwe_assert.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace {

LWELogLevel g_log_level = LWELogLevel_Warning;

char const *GetLogLevelName(LWELogLevel level) {
  switch (level) {
  case LWELogLevel_Fatal:   return "FTL";
  case LWELogLevel_Error:   return "ERR";
  case LWELogLevel_Warning: return "WRN";
  case LWELogLevel_Debug:   return "DBG";
  case LWELogLevel_Info:    return "INF";
  case LWELogLevel_Verbose: return "VRB";
  default: assert(false);   return "";
  }
}

} // namespace

void LWESetLogLevel(LWELogLevel log_level) {
  g_log_level = log_level;
}

void LWELogMessage(LWELogLevel level, char const *format, ...) {
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

  LWE_ASSERT(LWELogLevel_Fatal != level);
}
