#pragma once

#include <lwe_graphics.h>

#if LWE_PLATFORM_OSX
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>

namespace lwe {

class GLGraphicsSystem : public IGraphicsSystem, public std::enable_shared_from_this<GLGraphicsSystem> {
public:
  GLGraphicsSystem()  = default;
  ~GLGraphicsSystem();

  bool Initialize();

  virtual GraphicsAPI API() const override { return GraphicsAPI::OpenGL; }

  virtual std::vector<GraphicsDeviceDescription> const &GraphicsDevices() const override { return devices_; }

  virtual bool CreateDevice(uint32_t device_index, std::shared_ptr<IGraphicsDevice> &out_device) override;

private:
  std::vector<GraphicsDeviceDescription> devices_;

#if LWE_PLATFORM_OSX
  CGLPixelFormatObj pixel_format_ = nullptr;
#endif
};

} // namespace lwe

#endif
