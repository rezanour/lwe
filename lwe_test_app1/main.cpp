#include <lwe_logging.h>
#include <lwe_graphics.h>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, INT show_command) {
  UNREFERENCED_PARAMETER(instance);
  UNREFERENCED_PARAMETER(prev_instance);
  UNREFERENCED_PARAMETER(command_line);
  UNREFERENCED_PARAMETER(show_command);

  lwe::SetLogLevel(lwe::LogLevel::Info);

  std::shared_ptr<lwe::IGraphicsSystem> graphics_system;
  if (!lwe::GraphicsSystemCreate(lwe::GraphicsAPI::Vulkan, graphics_system)) {
    LWE_LOG(Fatal, "Could not initialize graphics subsystem.");
  }

  return 0;
}

#elif defined(LWE_PLATFORM_OSX)

int main(int argc, char *argv[]) {
  lwe::SetLogLevel(lwe::LogLevel::Info);

  std::shared_ptr<lwe::IGraphicsSystem> graphics_system;
  if (!lwe::GraphicsSystemCreate(lwe::GraphicsAPI::Unknown, graphics_system)) {
    LWE_LOG(Fatal, "Could not initialize graphics subsystem.");
  }

  return 0;
}

#endif
