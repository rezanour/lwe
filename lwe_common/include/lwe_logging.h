#pragma once

#include <lwe_platform.h>

#include <stdarg.h>

typedef enum LWELogLevel {
  LWELogLevel_Fatal = 0,
  LWELogLevel_Error,
  LWELogLevel_Warning,
  LWELogLevel_Debug,
  LWELogLevel_Info,
  LWELogLevel_Verbose,
} LWELogLevel;

void LWESetLogLevel(LWELogLevel log_level);
void LWELogMessage(LWELogLevel level, char const *format, ...);

#define LWE_LOG(level, format, ...) \
  LWELogMessage(LWELogLevel_##level, "%s: %s (%d)\n     " format, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__);
