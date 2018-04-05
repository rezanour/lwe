#include <lwe_logging.h>

#include <gl/lwe_gl_graphics_device.h>

namespace lwe {

GLGraphicsDevice::GLGraphicsDevice(std::shared_ptr<GLGraphicsSystem> const &system)
  : system_(system) {
}

GLGraphicsDevice::~GLGraphicsDevice() {
#if LWE_PLATFORM_OSX
  if (context_) {
    if (context_ == CGLGetCurrentContext()) {
      CGLSetCurrentContext(nullptr);
    }
    CGLDestroyContext(context_);
    context_ = nullptr;
  }
#endif
}

#if LWE_PLATFORM_OSX
bool GLGraphicsDevice::Initialize(CGLPixelFormatObj const pixel_format) {
  CGLError const create_context_error = CGLCreateContext(pixel_format, nullptr, &context_);
  if (kCGLNoError != create_context_error) {
    LWE_LOG(Error, "Failed to create GL context. (%d)", create_context_error);
    return false;
  }
  if (!MakeCurrent()) {
    LWE_LOG(Error, "Failed to make GL context current.");
    return false;
  }
  return true;
}
#else
bool GLGraphicsDevice::Initialize() {
  return true;
}
#endif

bool GLGraphicsDevice::MakeCurrent() {
#if LWE_PLATFORM_OSX
  if (prev_context_) {
    LWE_LOG(Error, "Calling MakeCurrent second time without calling ReleaseCurrent first.");
    return false;
  }

  CGLContextObj curr_context = CGLGetCurrentContext();
  if (context_ == curr_context) {
    LWE_LOG(Error, "Calling MakeCurrent second time without calling ReleaseCurrent first.");
    return false;
  }

  CGLError const set_context_error = CGLSetCurrentContext(context_);
  if (kCGLNoError != set_context_error) {
    LWE_LOG(Error, "Failed to make context current.");
    return false;
  }

  prev_context_ = curr_context;
#endif
  return true;
}

void GLGraphicsDevice::ReleaseCurrent() {
#if LWE_PLATFORM_OSX
  CGLSetCurrentContext(prev_context_);
  prev_context_ = nullptr;
#endif
}

} // namespace lwe
