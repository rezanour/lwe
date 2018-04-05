#pragma once

#include <lwe_graphics.h>

#include <gl/lwe_gl_graphics_system.h>

namespace lwe {

class GLGraphicsDevice : public IGraphicsDevice {
public:
  GLGraphicsDevice(std::shared_ptr<GLGraphicsSystem> const &system);
  virtual ~GLGraphicsDevice();

#if LWE_PLATFORM_OSX
  bool Initialize(CGLPixelFormatObj const pixel_format);
#else
  bool Initialize();
#endif

  bool MakeCurrent();
  void ReleaseCurrent();

  virtual GraphicsAPI API() const override { return GraphicsAPI::OpenGL; }

private:
  std::shared_ptr<GLGraphicsSystem> system_;
#if LWE_PLATFORM_OSX
  CGLContextObj context_      = nullptr;
  CGLContextObj prev_context_ = nullptr;  // previous context when we made current, if any
#endif
};

} // namespace lwe
