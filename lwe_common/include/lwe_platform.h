// platform macros and standardization
#pragma once

#include <stdint.h>

#ifdef _WIN32
#define LWE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define LWE_PLATFORM_OSX 1
#else
#error OS platform not yet supported
#endif

#ifdef _NDEBUG
#define LWE_BUILD_RELEASE 1
#else
#define LWE_BUILD_DEBUG 1
#endif
