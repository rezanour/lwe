// the GraphicsSystem class provides high level information about the graphics subsystems of the platform,
// including which APIs are supported and what physical graphics adapters are available as well as their properties.
#pragma once

#include <lwe_platform.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace lwe {

enum class GraphicsAPI {
  Unknown = 0,
#if LWE_PLATFORM_WINDOWS
  Vulkan,
#endif // LWE_PLATFORM_WINDOWS
};

enum class GraphicsDeviceType {
  Unknown = 0,
  Discrete,
  Integrated,
};

struct IGraphicsDevice {
  virtual ~IGraphicsDevice() = default;

  virtual std::string const  &Name()             const = 0;
  virtual GraphicsDeviceType  Type()             const = 0;
  virtual uint64_t            LocalMemoryBytes() const = 0;
  virtual uint64_t            TotalMemoryBytes() const = 0;
};

struct IGraphicsSystem {
  typedef std::vector<std::shared_ptr<IGraphicsDevice>> GraphicsDeviceList;

  virtual ~IGraphicsSystem() = default;

  virtual GraphicsAPI                API()             const = 0;
  virtual GraphicsDeviceList  const &GraphicsDevices() const = 0;
};

bool GraphicsSystemCreate(GraphicsAPI const graphics_api, std::shared_ptr<IGraphicsSystem> &out_system);

} // namespace lwe
