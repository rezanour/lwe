#pragma once

#include <lwe_graphics.h>

#include <vulkan.h>

namespace lwe {

class VulkanGraphicsSystem;

class VulkanGraphicsDevice : public IGraphicsDevice {
public:
  VulkanGraphicsDevice(VkPhysicalDevice const device);
  virtual ~VulkanGraphicsDevice();

  // IGraphicsDevice
  virtual std::string const  &Name()             const override { return device_name_; }
  virtual GraphicsDeviceType  Type()             const override { return device_type_; }
  virtual uint64_t            LocalMemoryBytes() const override { return local_memory_bytes_; }
  virtual uint64_t            TotalMemoryBytes() const override { return total_memory_bytes_; }

private:
  // cached properties
  std::string        device_name_;
  GraphicsDeviceType device_type_        = GraphicsDeviceType::Unknown;
  uint64_t           local_memory_bytes_ = 0;
  uint64_t           total_memory_bytes_ = 0;

  // vulkan objects
  VkPhysicalDevice                 device_            = nullptr;
  VkPhysicalDeviceProperties       properties_        = {};
  VkPhysicalDeviceMemoryProperties memory_properties_ = {};
};

class VulkanGraphicsSystem : public IGraphicsSystem {
public:
  VulkanGraphicsSystem()  = default;
  ~VulkanGraphicsSystem();

  bool Initialize();

  virtual GraphicsAPI                API()             const override { return GraphicsAPI::Vulkan; }
  virtual GraphicsDeviceList  const &GraphicsDevices() const override { return devices_; }

private:
  static bool EnumerateInstanceLayers(std::vector<VkLayerProperties> &out_layers);
  static bool EnumerateInstanceExtensions(char const *layer_name, std::vector<VkExtensionProperties> &out_extensions);
  static bool EnumeratePhysicalDevices(VkInstance const instance, std::vector<VkPhysicalDevice> &out_physical_devices);
  static bool EnumerateDeviceLayers(VkPhysicalDevice const physical_device, std::vector<VkLayerProperties> &out_layers);
  static bool EnumerateDeviceExtensions(VkPhysicalDevice const physical_device, char const *layer_name, std::vector<VkExtensionProperties> &out_extensions);
  static void EnumerateDeviceQueueFamilies(VkPhysicalDevice const physical_device, std::vector<VkQueueFamilyProperties> &out_families);

private:
  VkInstance         instance_ = nullptr;
  GraphicsDeviceList devices_;
};

} // namespace lwe
