#include <lwe_logging.h>

#include <vulkan/lwe_vulkan_graphics_system.h>

namespace lwe {

VulkanGraphicsDevice::VulkanGraphicsDevice(std::shared_ptr<VulkanGraphicsSystem> const &system, VkPhysicalDevice physical_device)
  : system_(system), physical_device_(physical_device) {
}

VulkanGraphicsDevice::~VulkanGraphicsDevice() {
  graphics_queue_       = nullptr;
  compute_queue_        = nullptr;
  transfer_queue_       = nullptr;
  sparse_binding_queue_ = nullptr;

  if (device_) {
    vkDestroyDevice(device_, nullptr);
    device_ = nullptr;
  }
}

bool VulkanGraphicsDevice::Initialize() {
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

  LWE_LOG(Info, "---------------------------");
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
  LWE_LOG(Info, "---------------------------");

  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

#if LWE_BUILD_DEBUG
  char const *validation_layers[] ={
    "VK_LAYER_LUNARG_core_validation",
    "VK_LAYER_LUNARG_parameter_validation",
    "VK_LAYER_LUNARG_standard_validation",
  };
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

  float const queue_priorities[4] ={ 1.0f, 1.0f, 1.0f, 1.0f };

  VkDeviceQueueCreateInfo queue_create_infos[4]{};
  uint32_t queue_family_indices[4]     ={ graphics_family_index, compute_family_index, transfer_family_index, sparse_binding_family_index };
  uint32_t queue_indices[4]            ={};
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

  VkResult result = vkCreateDevice(physical_device_, &device_create_info, nullptr, &device_);
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

bool VulkanGraphicsDevice::EnumerateDeviceLayers(VkPhysicalDevice const physical_device, std::vector<VkLayerProperties> &out_layers) {
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

bool VulkanGraphicsDevice::EnumerateDeviceExtensions(VkPhysicalDevice const physical_device, char const *layer_name, std::vector<VkExtensionProperties> &out_extensions) {
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

void VulkanGraphicsDevice::EnumerateDeviceQueueFamilies(VkPhysicalDevice const physical_device, std::vector<VkQueueFamilyProperties> &out_families) {
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);

  out_families.resize(count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, out_families.data());
}


VulkanGraphicsSystem::~VulkanGraphicsSystem() {
  devices_.clear();

  if (instance_) {
    vkDestroyInstance(instance_, nullptr);
    instance_ = nullptr;
  }
}

bool VulkanGraphicsSystem::CreateDevice(uint32_t device_index, std::shared_ptr<IGraphicsDevice> &out_device) {
  if (device_index >= devices_.size()) {
    LWE_LOG(Error, "device_index out of range.");
    return false;
  }

  std::shared_ptr<VulkanGraphicsDevice> device = std::make_shared<VulkanGraphicsDevice>(shared_from_this(), reinterpret_cast<VkPhysicalDevice>(devices_[device_index].Handle));
  if (device->Initialize()) {
    out_device = device;
    return true;
  }

  return false;
}

bool VulkanGraphicsSystem::Initialize() {
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

#if LWE_BUILD_DEBUG
  char const *validation_layers[] ={
    "VK_LAYER_LUNARG_core_validation",
    "VK_LAYER_LUNARG_parameter_validation",
    "VK_LAYER_LUNARG_standard_validation",
  };
  instance_create_info.enabledLayerCount = 3;
  instance_create_info.ppEnabledLayerNames = validation_layers;
#endif

#ifdef LWE_PLATFORM_WINDOWS
  char const *required_instance_extensions[] ={
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
  for (auto const &physical_device : physical_devices) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    VkPhysicalDeviceMemoryProperties memory_properties{};
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    GraphicsDeviceDescription desc{};
    //desc.API = API();
    desc.Handle = reinterpret_cast<uint64_t>(physical_device);
    strcpy(desc.Name, properties.deviceName);
    switch (properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   desc.Type = GraphicsDeviceType::Discrete;   break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: desc.Type = GraphicsDeviceType::Integrated; break;
    default:
      LWE_LOG(Warning, "Unsupported device type: %d", properties.deviceType);
      desc.Type = GraphicsDeviceType::Unknown;
      break;
    }
    for (uint32_t i = 0; i < memory_properties.memoryHeapCount; ++i) {
      if (VK_MEMORY_HEAP_DEVICE_LOCAL_BIT & memory_properties.memoryHeaps[i].flags) {
        desc.LocalMemoryBytes += memory_properties.memoryHeaps[i].size;
      }
    }

    devices_.push_back(desc);
    LWE_LOG(Info, "Added device: %s, Local Memory: %3.2fGB", desc.Name, desc.LocalMemoryBytes / 1024.0f / 1024.0f / 1024.0f);
  }

  LWE_LOG(Info, "---------------------------");

  return true;
}

bool VulkanGraphicsSystem::EnumerateInstanceLayers(std::vector<VkLayerProperties> &out_layers) {
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

bool VulkanGraphicsSystem::EnumerateInstanceExtensions(char const *layer_name, std::vector<VkExtensionProperties> &out_extensions) {
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

bool VulkanGraphicsSystem::EnumeratePhysicalDevices(VkInstance const instance, std::vector<VkPhysicalDevice> &out_physical_devices) {
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

} // namespace lwe
