#include <lwe_graphics.h>
#include <lwe_logging.h>

#if LWE_PLATFORM_WINDOWS
#include <vulkan/lwe_vulkan_graphics_system.h>
#endif

#include <gl/lwe_gl_graphics_system.h>

namespace lwe {

bool IGraphicsSystem::Create(GraphicsAPI const graphics_api, std::shared_ptr<IGraphicsSystem> &out_system) {
  switch (graphics_api) {
  case GraphicsAPI::Vulkan:
  {
#if LWE_PLATFORM_WINDOWS
    std::shared_ptr<VulkanGraphicsSystem> vulkan_system = std::make_shared<VulkanGraphicsSystem>();
    if (!vulkan_system->Initialize()) {
      LWE_LOG(Error, "Vulkan Graphics System failed to initialize.");
      return false;
    }
    out_system = vulkan_system;
    return true;
#else
    LWE_LOG(Error, "Vulkan is not available on this platform.");
    return false;
#endif
  }
  case GraphicsAPI::OpenGL:
  {
    std::shared_ptr<GLGraphicsSystem> gl_system = std::make_shared<GLGraphicsSystem>();
    if (!gl_system->Initialize()) {
      LWE_LOG(Error, "GL Graphics System failed to initialize.");
      return false;
    }
    out_system = gl_system;
    return true;
  }
  default:
    LWE_LOG(Error, "Invalid or unknown GraphicsAPI parameter.");
    return false;
  }
}

} // namespace lwe
