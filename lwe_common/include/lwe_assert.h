// assert support
#pragma once

#include <lwe_platform.h>

#if LWE_PLATFORM_WINDOWS
#include <assert.h>
#define LWE_ASSERT(x) assert(x)
#endif