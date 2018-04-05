#include <lwe_logging.h>

#include <gl/lwe_gl_render_backend.h>

namespace lwe {

GLRenderBackend::GLRenderBackend(std::shared_ptr<GLGraphicsDevice> const &device)
  : device_(device) {
}

GLRenderBackend::~GLRenderBackend() {
}

bool GLRenderBackend::Initialize() {
  return true;
}

} // namespace lwe
