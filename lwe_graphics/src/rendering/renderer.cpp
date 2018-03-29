#include <lwe_logging.h>
#include <lwe_renderer.h>

Renderer::Renderer() {
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    std::unique_lock<std::mutex> lock(init_mutex_);

    thread_ = std::thread([&]() { ThreadProc(); });
    init_cond_var_.wait(lock);

    return init_completed_;
}

void Renderer::Shutdown() {
    if (thread_.joinable()) {
        exit_requested_ = true;
        thread_.join();
    }
}

void Renderer::ThreadProc() {
    init_completed_ = RenderThreadInit();
    init_cond_var_.notify_all();

    if (!init_completed_) {
#ifdef LWE_PLATFORM_WINDOWS
        VulkanShutdown();
#endif // LWE_PLATFORM_WINDOWS
        return;
    }

    while (!exit_requested_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

#ifdef LWE_PLATFORM_WINDOWS
    VulkanShutdown();
#endif // LWE_PLATFORM_WINDOWS
}

bool Renderer::RenderThreadInit() {
#ifdef LWE_PLATFORM_WINDOWS
    if (!VulkanInitialize()) {
        LWE_LOG(Debug, "Vulkan Initialization Failed.");
        return false;
    }
#endif // LWE_PLATFORM_WINDOWS

    return true;
}

#ifdef LWE_PLATFORM_WINDOWS
bool Renderer::EnumerateInstanceLayers(std::vector<VkLayerProperties> &out_layers) {
    uint32_t count = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr);
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to query number of layers (%d)", result);
        return false;
    }

    out_layers.resize(count);
    result = vkEnumerateInstanceLayerProperties(&count, out_layers.data());
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to enumerate layers (%d)", result);
        return false;
    }

    return true;
}

bool Renderer::EnumerateInstanceExtensions(char const *layer_name, std::vector<VkExtensionProperties> &out_extensions) {
    uint32_t count = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(layer_name, &count, nullptr);
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to query number of extensions%s%s (%d)",
            layer_name ? " for layer " : "",
            layer_name ? layer_name : "",
            result);
        return false;
    }

    out_extensions.resize(count);
    result = vkEnumerateInstanceExtensionProperties(layer_name, &count, out_extensions.data());
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to enumerate extensions%s%s (%d)",
            layer_name ? " for layer " : "",
            layer_name ? layer_name : "",
            result);
        return false;
    }

    return true;
}

bool Renderer::EnumeratePhysicalDevices(VkInstance const instance, std::vector<VkPhysicalDevice> &out_physical_devices) {
    uint32_t count = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &count, nullptr);
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to query number of physical devices (%d)", result);
        return false;
    }

    out_physical_devices.resize(count);
    result = vkEnumeratePhysicalDevices(instance, &count, out_physical_devices.data());
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to enumerate physical devices (%d)", result);
        return false;
    }

    return true;
}

bool Renderer::EnumerateDeviceLayers(VkPhysicalDevice const physical_device, std::vector<VkLayerProperties> &out_layers) {
    uint32_t count = 0;
    VkResult result = vkEnumerateDeviceLayerProperties(physical_device, &count, nullptr);
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to query number of device layers (%d)", result);
        return false;
    }

    out_layers.resize(count);
    result = vkEnumerateDeviceLayerProperties(physical_device, &count, out_layers.data());
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to enumerate device layers (%d)", result);
        return false;
    }

    return true;
}

bool Renderer::EnumerateDeviceExtensions(VkPhysicalDevice const physical_device, char const *layer_name, std::vector<VkExtensionProperties> &out_extensions) {
    uint32_t count = 0;
    VkResult result = vkEnumerateDeviceExtensionProperties(physical_device, layer_name, &count, nullptr);
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to query number of device extensions%s%s (%d)",
            layer_name ? " for layer " : "",
            layer_name ? layer_name : "",
            result);
        return false;
    }

    out_extensions.resize(count);
    result = vkEnumerateDeviceExtensionProperties(physical_device, layer_name, &count, out_extensions.data());
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to enumerate device extensions%s%s (%d)",
            layer_name ? " for layer " : "",
            layer_name ? layer_name : "",
            result);
        return false;
    }

    return true;
}

void Renderer::EnumerateDeviceQueueFamilies(VkPhysicalDevice const physical_device, std::vector<VkQueueFamilyProperties> &out_families) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);

    out_families.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, out_families.data());
}

bool Renderer::VulkanInitialize() {
    std::vector<VkLayerProperties> instance_layers;
    std::vector<VkExtensionProperties> instance_extensions;
    std::map<char const *, std::vector<VkExtensionProperties>> instance_layer_extensions;

    if (!EnumerateInstanceLayers(instance_layers)) {
        return false;
    }
    if (!EnumerateInstanceExtensions(nullptr, instance_extensions)) {
        return false;
    }
    for (auto &layer : instance_layers) {
        if (!EnumerateInstanceExtensions(layer.layerName, instance_layer_extensions[layer.layerName])) {
            return false;
        }
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Lightweight Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = "LWE Test";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;

#ifndef _NDEBUG
    char const *validation_layers[] = {
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_standard_validation",
    };
    instance_create_info.enabledLayerCount = 3;
    instance_create_info.ppEnabledLayerNames = validation_layers;
#endif

#ifdef LWE_PLATFORM_WINDOWS
    char const *required_instance_extensions[] = {
        "VK_EXT_debug_report",
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
    };
    instance_create_info.enabledExtensionCount = 3;
    instance_create_info.ppEnabledExtensionNames = required_instance_extensions;
#else
#endif

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_);
    if (VK_SUCCESS != result) {
      LWE_LOG(Error, "Failed to create VK instance (%d).", result);
      return false;
    }

    std::vector<VkPhysicalDevice> physical_devices;
    if (!EnumeratePhysicalDevices(instance_, physical_devices)) {
        return false;
    }

    LWE_LOG(Info, "Vulkan Initialized");
    LWE_LOG(Info, "---------------------------");
    LWE_LOG(Info, "  Extensions:");
    for (auto const &extension : instance_extensions) {
        LWE_LOG(Info, "    %s", extension.extensionName);
    }

    LWE_LOG(Info, "");

    LWE_LOG(Info, "  Layers:");
    for (auto const &layer : instance_layers) {
        LWE_LOG(Info, "    %s: %s", layer.layerName, layer.description);
        for (auto const &extension : instance_layer_extensions[layer.layerName]) {
            LWE_LOG(Info, "      %s", extension.extensionName);
        }
    }

    LWE_LOG(Info, "");

    uint64_t const mb_to_bytes = 1024 * 1024;
    uint64_t const minimum_required_memory_bytes = 4096 * mb_to_bytes;

    for (auto const &physical_device : physical_devices) {
        VkPhysicalDeviceMemoryProperties memory_properties{};
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
        VkDeviceSize total_local_memory = 0;
        for (uint32_t mem_index = 0; mem_index < memory_properties.memoryHeapCount; ++mem_index) {
            if (memory_properties.memoryHeaps[mem_index].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                total_local_memory += memory_properties.memoryHeaps[mem_index].size;
            }
        }

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physical_device, &properties);
        LWE_LOG(Info, "  Physical Device: %s%s, Local Memory: %lluMB",
            properties.deviceName,
            (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == properties.deviceType) ? " (discrete)" : "",
            total_local_memory / mb_to_bytes);

        // if we find a discrete with enough memory, that is the preferred option,
        // so select it and bail.
        if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == properties.deviceType && total_local_memory >= minimum_required_memory_bytes) {
            physical_device_            = physical_device;
            physical_device_properties_ = properties;
            physical_device_memory_     = memory_properties;
            LWE_LOG(Info, "  Device candidate found %s", properties.deviceName);
            break;
        }

        // otherwise, if we find an integrated GPU with enough memory, that is second best.
        // so select it, but don't bail in case we find a discrete while continuing through
        // available devices.
        if (VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU == properties.deviceType && total_local_memory >= minimum_required_memory_bytes) {
            physical_device_            = physical_device;
            physical_device_properties_ = properties;
            physical_device_memory_     = memory_properties;
            LWE_LOG(Info, "  Device candidate found: %s", properties.deviceName);
        }
    }
    LWE_LOG(Info, "");
    if (!physical_device_) {
        LWE_LOG(Error, "  No suitable device found!");
        return false;
    }

    LWE_LOG(Info, "  * Device selected: %s", physical_device_properties_.deviceName);

    std::vector<VkLayerProperties> device_layers;
    std::vector<VkExtensionProperties> device_extensions;
    std::map<char const *, std::vector<VkExtensionProperties>> device_layer_extensions;

    if (!EnumerateDeviceLayers(physical_device_, device_layers)) {
        return false;
    }
    if (!EnumerateDeviceExtensions(physical_device_, nullptr, device_extensions)) {
        return false;
    }
    for (auto &layer : device_layers) {
        if (!EnumerateDeviceExtensions(physical_device_, layer.layerName, device_layer_extensions[layer.layerName])) {
            return false;
        }
    }

    LWE_LOG(Info, "");
    LWE_LOG(Info, "  Device Extensions:");
    for (auto const &extension : device_extensions) {
        LWE_LOG(Info, "    %s", extension.extensionName);
    }

    LWE_LOG(Info, "");

    LWE_LOG(Info, "  Device Layers:");
    for (auto const &layer : device_layers) {
        LWE_LOG(Info, "    %s: %s", layer.layerName, layer.description);
        for (auto const &extension : device_layer_extensions[layer.layerName]) {
            LWE_LOG(Info, "      %s", extension.extensionName);
        }
    }

    LWE_LOG(Info, "---------------------------");

    std::vector<VkQueueFamilyProperties> queue_families;
    EnumerateDeviceQueueFamilies(physical_device_, queue_families);

    uint32_t graphics_family_index       = 0;
    uint32_t compute_family_index        = 0;
    uint32_t transfer_family_index       = 0;
    uint32_t sparse_binding_family_index = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families.size()); ++i) {
        auto const &family = queue_families[i];

        std::string queue_type;
        if (VK_QUEUE_GRAPHICS_BIT & family.queueFlags) {
            queue_type += "|graphics";
            graphics_family_index = i;
        }
        if (VK_QUEUE_COMPUTE_BIT & family.queueFlags) {
            queue_type += "|compute";
            compute_family_index = i;
        }
        if (VK_QUEUE_TRANSFER_BIT & family.queueFlags) {
            queue_type += "|transfer";
            transfer_family_index = i;
        }
        if (VK_QUEUE_SPARSE_BINDING_BIT & family.queueFlags) {
            queue_type += "|sparse binding";
            sparse_binding_family_index = i;
        }
        queue_type += "|";

        LWE_LOG(Info, "Queue: %s (%d)", queue_type.c_str(), family.queueCount);
    }

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

#ifndef _NDEBUG
    device_create_info.enabledLayerCount = 3;
    device_create_info.ppEnabledLayerNames = validation_layers;
#endif

#ifdef LWE_PLATFORM_WINDOWS
    char const *required_device_extensions[] ={
        "VK_KHR_swapchain",
    };
    device_create_info.enabledExtensionCount = 1;
    device_create_info.ppEnabledExtensionNames = required_device_extensions;
#else
#endif

    float const queue_priorities[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    VkDeviceQueueCreateInfo queue_create_infos[4]{};
    uint32_t queue_family_indices[4]     = { graphics_family_index, compute_family_index, transfer_family_index, sparse_binding_family_index };
    uint32_t queue_indices[4]            = {};
    uint32_t queue_families_required     = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        // is someone already using this queue family? Then add onto it
        bool skip_add = false;
        for (uint32_t j = 0; j < queue_families_required; ++j) {
            if (queue_create_infos[j].queueFamilyIndex == queue_family_indices[i]) {
                queue_indices[i] = queue_create_infos[j].queueCount++;
                skip_add = true;
                break;
            }
        }
        if (!skip_add) {
            queue_create_infos[queue_families_required].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[queue_families_required].queueFamilyIndex = queue_family_indices[i];
            queue_create_infos[queue_families_required].queueCount       = 1;
            queue_create_infos[queue_families_required].pQueuePriorities = queue_priorities;
            ++queue_families_required;
        }
    }

    device_create_info.queueCreateInfoCount = queue_families_required;
    device_create_info.pQueueCreateInfos = queue_create_infos;

    result = vkCreateDevice(physical_device_, &device_create_info, nullptr, &device_);
    if (VK_SUCCESS != result) {
        LWE_LOG(Error, "Failed to create vulkan device (%d)", result);
        return false;
    }

    vkGetDeviceQueue(device_, queue_family_indices[0], queue_indices[0], &graphics_queue_);
    vkGetDeviceQueue(device_, queue_family_indices[1], queue_indices[1], &compute_queue_);
    vkGetDeviceQueue(device_, queue_family_indices[2], queue_indices[2], &transfer_queue_);
    vkGetDeviceQueue(device_, queue_family_indices[3], queue_indices[3], &sparse_binding_queue_);
    if (!graphics_queue_ || !compute_queue_ || !transfer_queue_ || !sparse_binding_queue_) {
        LWE_LOG(Error, "Failed to get queue handles.");
        return false;
    }

    return true;
}

void Renderer::VulkanShutdown() {
    vkDestroyDevice(device_, nullptr);
    vkDestroyInstance(instance_, nullptr);

    sparse_binding_queue_ = nullptr;
    transfer_queue_       = nullptr;
    compute_queue_        = nullptr;
    graphics_queue_       = nullptr;
    device_               = nullptr;
    physical_device_      = nullptr;
    instance_             = nullptr;
}
#endif // LWE_PLATFORM_WINDOWS
