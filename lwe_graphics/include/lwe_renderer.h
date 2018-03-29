#pragma once

#include <lwe_platform.h>
#pragma warning (push)
#pragma warning (disable: 4251)
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#ifdef LWE_PLATFORM_WINDOWS
#include <vulkan.h>
#endif

class Renderer {
public:
    Renderer();
    virtual ~Renderer();

    bool Initialize();
    void Shutdown();

private:
    void ThreadProc();
    bool RenderThreadInit();
#ifdef LWE_PLATFORM_WINDOWS
    bool VulkanInitialize();
    void VulkanShutdown();

    static bool EnumerateInstanceLayers(std::vector<VkLayerProperties> &out_layers);
    static bool EnumerateInstanceExtensions(char const *layer_name, std::vector<VkExtensionProperties> &out_extensions);
    static bool EnumeratePhysicalDevices(VkInstance const instance, std::vector<VkPhysicalDevice> &out_physical_devices);
    static bool EnumerateDeviceLayers(VkPhysicalDevice const physical_device, std::vector<VkLayerProperties> &out_layers);
    static bool EnumerateDeviceExtensions(VkPhysicalDevice const physical_device, char const *layer_name, std::vector<VkExtensionProperties> &out_extensions);
    static void EnumerateDeviceQueueFamilies(VkPhysicalDevice const physical_device, std::vector<VkQueueFamilyProperties> &out_families);
#endif // _WIN32

private:
    std::thread             thread_;
    std::condition_variable init_cond_var_;
    std::mutex              init_mutex_;
    bool                    init_completed_ = false;
    std::atomic_bool        exit_requested_ = ATOMIC_VAR_INIT(false);

#ifdef LWE_PLATFORM_WINDOWS
    // Vulkan objects
    VkInstance                       instance_                   = nullptr;
    VkPhysicalDevice                 physical_device_            = nullptr;
    VkPhysicalDeviceProperties       physical_device_properties_ = {};
    VkPhysicalDeviceMemoryProperties physical_device_memory_     = {};
    VkDevice                         device_                     = nullptr;
    VkQueue                          graphics_queue_             = nullptr;
    VkQueue                          compute_queue_              = nullptr;
    VkQueue                          transfer_queue_             = nullptr;
    VkQueue                          sparse_binding_queue_       = nullptr;
#endif // _WIN32
};

#pragma warning (pop)
