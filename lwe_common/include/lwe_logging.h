#pragma once

#include <lwe_platform.h>

namespace lwe {

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

#define LWE_LOG(level, format, ...) LogMessage(lwe::LogLevel::##level, format, __VA_ARGS__)

} // namespace lwe
