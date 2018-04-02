#include <lwe_graphics.h>
#include <lwe_logging.h>

#if LWE_PLATFORM_WINDOWS
#include <graphics_systems/lwe_vulkan_graphics_system.h>
#endif

namespace lwe {

bool GraphicsSystemCreate(GraphicsAPI const graphics_api, std::shared_ptr<IGraphicsSystem> &out_system) {
#if LWE_PLATFORM_WINDOWS
  if (GraphicsAPI::Vulkan == graphics_api) {
    std::shared_ptr<VulkanGraphicsSystem> vulkan_system = std::make_shared<VulkanGraphicsSystem>();
    if (!vulkan_system->Initialize()) {
      LWE_LOG(Info, "Vulkan Graphics System failed to initialize.");
      return false;
    }
    out_system = std::dynamic_pointer_cast<IGraphicsSystem>(vulkan_system);
    return true;
  }
#endif // LWE_PLATFORM_WINDOWS

  LWE_LOG(Error, "Invalid or unknown GraphicsAPI parameter.");
  return false;
}

} // namespace lwe
