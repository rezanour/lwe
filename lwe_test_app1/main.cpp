#include <lwe_logging.h>
#include <lwe_graphics.h>
#include <lwe_system.h>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, INT show_command) {
  UNREFERENCED_PARAMETER(instance);
  UNREFERENCED_PARAMETER(prev_instance);
  UNREFERENCED_PARAMETER(command_line);
  UNREFERENCED_PARAMETER(show_command);

  lwe::SetLogLevel(lwe::LogLevel::Info);

  if (!lwe::System::Get().Initialize()) {
    LWE_LOG(Error, "Failed to initialize system");
    return -1;
  }

  LWE_LOG(Info, "Logical Cores: %u", lwe::System::Get().LogicalCPUCount());
  LWE_LOG(Info, "Total System Memory: %3.2fGB", lwe::System::Get().TotalSystemMemoryBytes() / 1024.0f / 1024.0f / 1024.0f);
  LWE_LOG(Info, "Available System Memory: %3.2fGB", lwe::System::Get().AvailableSystemMemoryBytes() / 1024.0f / 1024.0f / 1024.0f);
  LWE_LOG(Info, "Vulkan Available: %s", lwe::System::Get().VulkanAvailable() ? "yes" : "no");

  if (lwe::System::Get().VulkanAvailable()) {
    std::shared_ptr<lwe::IGraphicsSystem> graphics_system;
    if (!lwe::GraphicsSystemCreate(lwe::GraphicsAPI::Vulkan, graphics_system)) {
      LWE_LOG(Fatal, "Could not initialize graphics subsystem.");
      return -2;
    }

    std::shared_ptr<lwe::IGraphicsDevice> device;
    if (!graphics_system->CreateDevice(0, device)) {
      LWE_LOG(Fatal, "Failed to create graphics device.");
      return -3;
    }
  }

  return 0;
}

#elif defined(LWE_PLATFORM_OSX)

int main(int argc, char *argv[]) {
  lwe::SetLogLevel(lwe::LogLevel::Info);

  std::shared_ptr<lwe::IGraphicsSystem> graphics_system;
  if (!lwe::GraphicsSystemCreate(lwe::GraphicsAPI::Vulkan, graphics_system)) {
    LWE_LOG(Fatal, "Could not initialize graphics subsystem.");
  }

  return 0;
}

#endif
