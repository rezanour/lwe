// the GraphicsSystem class provides high level information about the graphics subsystem of the machine,
// including which APIs are supported and what physical graphics adapters there are and their properties.
#pragma once

#include <stdint.h>
#include <memory>
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

struct IGraphicsDevice {
  virtual ~IGraphicsDevice() {}

  virtual GraphicsDeviceType Type()        const = 0;
  virtual uint64_t           LocalMemory() const = 0;
  virtual uint64_t           TotalMemory() const = 0;
};

class GraphicsSystem {
public:
  ~GraphicsSystem();

  // obtain the singleton reference
  static bool Create(std::shared_ptr<GraphicsSystem> &out_system);

  // a bitmask of supported graphics APIs on this system
  GraphicsAPI SupportedGraphicsAPIs() const { return supported_apis_; }

  // a list of available physical graphics devices/adapters
  std::vector<std::shared_ptr<IGraphicsDevice>> const &GraphicsDevices() const { return devices_; }

private:
  GraphicsSystem()  = default;
  bool Initialize();

private:
  GraphicsAPI                                   supported_apis_ = GraphicsAPI::Unknown;
  std::vector<std::shared_ptr<IGraphicsDevice>> devices_;
};

} // namespace lwe
