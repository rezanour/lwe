#include <lwe_graphics.h>
#include <lwe_render_backend.h>
#include <lwe_logging.h>

#include <gl/lwe_gl_graphics_system.h>
#include <gl/lwe_gl_render_backend.h>

namespace lwe {

bool IRenderBackend::Create(std::shared_ptr<IGraphicsDevice> const &device, std::shared_ptr<IRenderBackend> &out_backend) {
  if (!device) {
    LWE_LOG(Error, "null device provided.");
    return false;
  }

  switch (device->API()) {
  case GraphicsAPI::Vulkan:
#if LWE_PLATFORM_WINDOWS
  {
    auto vulkan_device = std::dynamic_pointer_cast<VulkanGraphicsDevice>(device);
    if (!vulkan_device) {
      LWE_LOG(Error, "Invalid device provided.");
      return false;
    }
    break;
  }
#else
    LWE_LOG(Error, "Vulkan not supported on this platform.");
    return false;
#endif
  case GraphicsAPI::OpenGL:
  {
    auto gl_device = std::dynamic_pointer_cast<GLGraphicsDevice>(device);
    if (!gl_device) {
      LWE_LOG(Error, "Invalid device provided.");
      return false;
    }
    auto gl_backend = std::make_shared<GLRenderBackend>(gl_device);
    if (!gl_backend->Initialize()) {
      LWE_LOG(Error, "Failed to initialize render backend.");
      return false;
    }
    out_backend = gl_backend;
    return true;
  }
  default:
    LWE_LOG(Error, "Unknown device provided.");
    return false;
  }

  return false;
}

} // namespace lwe
