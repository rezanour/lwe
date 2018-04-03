#include <lwe_logging.h>
#include <lwe_system.h>

#include <mutex>
#include <vector>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace lwe {

namespace {
  std::once_flag g_once_flag;
}

std::unique_ptr<System> System::s_instance_;

System &System::Get() {
  std::call_once(g_once_flag, []() { s_instance_.reset(new System()); });
  return *s_instance_.get();
}

uint64_t System::AvailableSystemMemoryBytes() const {
#if LWE_PLATFORM_WINDOWS
  MEMORYSTATUSEX status{};
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  return status.ullAvailPhys;
#elif LWE_PLATFORM_OSX
  return 0;
#endif
}

bool System::Initialize() {
  if (initialized_) {
    return initialized_;
  }

  logical_cpu_count_ = 0;

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
      logical_cpu_count_ += __popcnt(static_cast<uint32_t>(p->ProcessorMask));
    }
    offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    ++p;
  }

  MEMORYSTATUSEX status{};
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  total_system_memory_bytes_ = status.ullTotalPhys;

  HMODULE module = LoadLibraryA("vulkan-1.dll");
  if (module) {
    vulkan_available_ = true;
    FreeLibrary(module);
    module = nullptr;
  }

  initialized_ = true;

#elif LWE_PLATFORM_OSX
  logical_cpu_count_ = 0;
  total_system_memory_ = 0;
  vulkan_available_ = false;
  initialized_ = true;
#endif

  return initialized_;
}

} // namespace lwe
