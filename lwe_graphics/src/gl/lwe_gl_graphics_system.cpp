#include <lwe_logging.h>

#include <gl/lwe_gl_graphics_device.h>
#include <gl/lwe_gl_graphics_system.h>

namespace lwe {

GLGraphicsSystem::~GLGraphicsSystem() {
  devices_.clear();

#if LWE_PLATFORM_OSX
  if (pixel_format_) {
    CGLDestroyPixelFormat(pixel_format_);
    pixel_format_ = nullptr;
  }
#endif
}

bool GLGraphicsSystem::CreateDevice(uint32_t device_index, std::shared_ptr<IGraphicsDevice> &out_device) {
  if (device_index >= devices_.size()) {
    LWE_LOG(Error, "device_index out of range.");
    return false;
  }

  std::shared_ptr<GLGraphicsDevice> device = std::make_shared<GLGraphicsDevice>(shared_from_this());
#if LWE_PLATFORM_OSX
  if (device->Initialize(pixel_format_)) {
#else
  if (device->Initialize()) {
#endif
    out_device = device;
    return true;
  }

  return false;
}

bool GLGraphicsSystem::Initialize() {
#if LWE_PLATFORM_OSX
  CGLPixelFormatAttribute const pixel_format_attributes[] = {
    kCGLPFAAccelerated,
    kCGLPFATripleBuffer,
    kCGLPFAColorSize, static_cast<CGLPixelFormatAttribute>(24),
    kCGLPFAAlphaSize, static_cast<CGLPixelFormatAttribute>(8),
    kCGLPFADepthSize, static_cast<CGLPixelFormatAttribute>(32),
    static_cast<CGLPixelFormatAttribute>(0),
  };

  int32_t num_formats = 1;
  CGLError const choose_pixel_format_error = CGLChoosePixelFormat(pixel_format_attributes, &pixel_format_, &num_formats);
  if (kCGLNoError != choose_pixel_format_error) {
    LWE_LOG(Error, "Failed to find suitable pixel format (%d).", choose_pixel_format_error);
    return false;
  }

#endif // LWE_PLATFORM_OSX

  GraphicsDeviceDescription desc{};
  desc.Handle = 0;
  strcpy(desc.Name, "Default GPU");
  desc.Type = GraphicsDeviceType::Discrete;
  desc.LocalMemoryBytes = 0;
  devices_.push_back(desc);

  return true;
}

} // namespace lwe
