#include <lwe_logging.h>
#include <lwe_system.h>

#include <mutex>
#include <vector>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>
#elif LWE_PLATFORM_OSX
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#endif

namespace {
  std::mutex           g_init_mutex;
  bool                 g_initialized = false;
  LWESystemInformation g_information = {};
}

bool LWESystemGetInformation(LWESystemInformation *out_info) {
  if (!g_initialized) {
    std::lock_guard<std::mutex> lock(g_init_mutex);
    if (!g_initialized) {
      g_information = {};

#if LWE_PLATFORM_WINDOWS
      DWORD buffer_size = 0;
      GetLogicalProcessorInformation(nullptr, &buffer_size);
      std::vector<uint8_t> buffer(buffer_size);
      SYSTEM_LOGICAL_PROCESSOR_INFORMATION *p = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION *>(buffer.data());
      if (!GetLogicalProcessorInformation(p, &buffer_size)) {
        LWE_LOG(Error, "Failed to get processor information.");
        return false;
      }

      uint64_t offset = 0;
      while (offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) < buffer_size) {
        if (RelationProcessorCore == p->Relationship) {
          g_information.logical_cpu_count += __popcnt(static_cast<uint32_t>(p->ProcessorMask));
        }
        offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ++p;
      }

      MEMORYSTATUSEX status{};
      status.dwLength = sizeof(status);
      if (!GlobalMemoryStatusEx(&status)) {
        LWE_LOG(Error, "Failed to get memory status.");
        return false;
      }
      g_information.total_system_memory_bytes = status.ullTotalPhys;

      HMODULE module = LoadLibraryA("vulkan-1.dll");
      if (module) {
        g_information.vulkan_available = true;
        FreeLibrary(module);
        module = nullptr;
      }

#elif LWE_PLATFORM_OSX
      size_t count_len = sizeof(g_information.logical_cpu_count);
      sysctlbyname("hw.logicalcpu", &g_information.logical_cpu_count, &count_len, nullptr, 0);

      int32_t mib[] ={
        CTL_HW,
        HW_MEMSIZE
      };
      size_t memory_len = sizeof(g_information.total_system_memory_bytes);
      sysctl(mib, 2, &g_information.total_system_memory_bytes, &memory_len, nullptr, 0);

      g_information.vulkan_available = false;
#endif

      g_initialized = true;
    }
  }

  *out_info = g_information;
  return true;
}
