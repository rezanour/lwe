// interface for render backend implementations (eg. Vulkan)
#pragma once

#include <lwe_platform.h>

#include <memory>

namespace lwe {

struct IGraphicsDevice;

struct IRenderBackend {
  static bool Create(std::shared_ptr<IGraphicsDevice> const &device, std::shared_ptr<IRenderBackend> &out_backend);

  virtual ~IRenderBackend() = default;
};

} // namespace lwe
