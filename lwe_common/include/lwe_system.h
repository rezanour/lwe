// system information
#pragma once

#include <lwe_platform.h>

typedef struct LWESystemInformation {
  uint32_t logical_cpu_count;
  uint64_t total_system_memory_bytes;
  bool     vulkan_available;
} LWESystemInformation;

bool LWESystemGetInformation(LWESystemInformation *out_info);
