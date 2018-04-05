#pragma once

#include <lwe_graphics.h>
#include <lwe_render_backend.h>

#include <gl/lwe_gl_graphics_device.h>

namespace lwe {

class GLRenderBackend : public IRenderBackend {
public:
  GLRenderBackend(std::shared_ptr<GLGraphicsDevice> const &device);
  virtual ~GLRenderBackend();

  bool Initialize();

private:
  std::shared_ptr<GLGraphicsDevice> device_;
};

} // namespace lwe
