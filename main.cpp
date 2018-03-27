#include <Windows.h>

#include <rendering/renderer.h>
#include <common/logging.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, INT show_command) {
    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(prev_instance);
    UNREFERENCED_PARAMETER(command_line);
    UNREFERENCED_PARAMETER(show_command);

    SetLogLevel(LogLevel::Info);

    Renderer renderer;
    renderer.Initialize();
    renderer.Shutdown();
    return 0;
}