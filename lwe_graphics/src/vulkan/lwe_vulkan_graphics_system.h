#pragma once

#include <lwe_graphics.h>

#include <vulkan.h>

namespace lwe {

class VulkanGraphicsSystem;

class VulkanGraphicsDevice : public IGraphicsDevice {
public:
  VulkanGraphicsDevice(std::shared_ptr<VulkanGraphicsSystem> const &system, VkPhysicalDevice physical_device);
  virtual ~VulkanGraphicsDevice();

  bool Initialize();

  virtual GraphicsAPI API() const override { return GraphicsAPI::Vulkan; }

private:
  static bool EnumerateDeviceLayers(VkPhysicalDevice const physical_device, std::vector<VkLayerProperties> &out_layers);
  static bool EnumerateDeviceExtensions(VkPhysicalDevice const physical_device, char const *layer_name, std::vector<VkExtensionProperties> &out_extensions);
  static void EnumerateDeviceQueueFamilies(VkPhysicalDevice const physical_device, std::vector<VkQueueFamilyProperties> &out_families);

private:
  std::shared_ptr<VulkanGraphicsSystem> system_;
  VkPhysicalDevice                      physical_device_      = nullptr;
  VkDevice                              device_               = nullptr;
  VkQueue                               graphics_queue_       = nullptr;
  VkQueue                               compute_queue_        = nullptr;
  VkQueue                               transfer_queue_       = nullptr;
  VkQueue                               sparse_binding_queue_ = nullptr;
};

class VulkanGraphicsSystem : public IGraphicsSystem, public std::enable_shared_from_this<VulkanGraphicsSystem> {
public:
  VulkanGraphicsSystem()  = default;
  ~VulkanGraphicsSystem();

  bool Initialize();

  virtual GraphicsAPI API() const override { return GraphicsAPI::Vulkan; }

  virtual std::vector<GraphicsDeviceDescription> const &GraphicsDevices() const override { return devices_; }

  virtual bool CreateDevice(uint32_t device_index, std::shared_ptr<IGraphicsDevice> &out_device) override;

private:
  static bool EnumerateInstanceLayers(std::vector<VkLayerProperties> &out_layers);
  static bool EnumerateInstanceExtensions(char const *layer_name, std::vector<VkExtensionProperties> &out_extensions);
  static bool EnumeratePhysicalDevices(VkInstance const instance, std::vector<VkPhysicalDevice> &out_physical_devices);

private:
  VkInstance instance_ = nullptr;

  std::vector<GraphicsDeviceDescription> devices_;
};

} // namespace lwe
