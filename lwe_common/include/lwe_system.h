// system information
#pragma once

#include <lwe_platform.h>

#include <memory>

namespace lwe {

class System {
public:
  static System &Get();
  ~System() = default;

  bool Initialize();

  uint32_t LogicalCPUCount()            const { return logical_cpu_count_; }
  uint64_t TotalSystemMemoryBytes()     const { return total_system_memory_bytes_; }
  uint64_t AvailableSystemMemoryBytes() const;
  bool     VulkanAvailable()            const { return vulkan_available_; }

private:
  System() = default;

private:
  static std::unique_ptr<System> s_instance_;

  bool     initialized_               = false;
  uint32_t logical_cpu_count_         = 0;
  uint64_t total_system_memory_bytes_ = 0;
  bool     vulkan_available_          = false;
};

} // namespace lwe
