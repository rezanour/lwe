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
  Vulkan,
};

enum class GraphicsDeviceType {
  Unknown = 0,
  Discrete,
  Integrated,
};

struct GraphicsDeviceDescription {
  GraphicsAPI        API;
  uint64_t           Handle;
  char               Name[128];
  GraphicsDeviceType Type;
  uint64_t           LocalMemoryBytes;
};

struct IGraphicsDevice {
  virtual ~IGraphicsDevice() = default;
};

struct IGraphicsSystem {
  virtual ~IGraphicsSystem() = default;

  virtual GraphicsAPI API() const = 0;

  virtual std::vector<GraphicsDeviceDescription> const &GraphicsDevices() const = 0;

  virtual bool CreateDevice(uint32_t device_index, std::shared_ptr<IGraphicsDevice> &out_device) = 0;
};

bool GraphicsSystemCreate(GraphicsAPI const graphics_api, std::shared_ptr<IGraphicsSystem> &out_system);

} // namespace lwe
