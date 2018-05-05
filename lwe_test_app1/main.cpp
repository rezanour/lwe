#include <lwe_logging.h>
#include <lwe_graphics.h>
#include <lwe_render_backend.h>
#include <lwe_system.h>

#if LWE_PLATFORM_WINDOWS
#include <Windows.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, INT show_command) {
  UNREFERENCED_PARAMETER(instance);
  UNREFERENCED_PARAMETER(prev_instance);
  UNREFERENCED_PARAMETER(command_line);
  UNREFERENCED_PARAMETER(show_command);

#elif LWE_PLATFORM_OSX
#if 0
void display(void) {
  //clear white, draw with black
  glClearColor(255, 255, 255, 0);
  glColor3d(0, 0, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //this draws a square using vertices
  glBegin(GL_QUADS);
  glVertex2i(0, 0);
  glVertex2i(0, 128);
  glVertex2i(128, 128);
  glVertex2i(128, 0);
  glEnd();

  //a more useful helper
  glRecti(200, 200, 250, 250);

  glutSwapBuffers();
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //set the coordinate system, with the origin in the top left
  gluOrtho2D(0, width, height, 0);
  glMatrixMode(GL_MODELVIEW);
}

void idle(void) {
  glutPostRedisplay();
}
#endif
int main(int argc, char *argv[]) {
#endif

  LWESetLogLevel(LWELogLevel_Info);

  LWESystemInformation system_info = {};
  if (!LWESystemGetInformation(&system_info)) {
    LWE_LOG(Error, "Failed to initialize system");
    return EXIT_FAILURE;
  }

  LWE_LOG(Info, "Logical Cores: %u", system_info.logical_cpu_count);
  LWE_LOG(Info, "Total System Memory: %3.2fGB", system_info.total_system_memory_bytes / 1024.0f / 1024.0f / 1024.0f);
  LWE_LOG(Info, "Vulkan Available: %s", system_info.vulkan_available ? "yes" : "no");

  lwe::GraphicsAPI graphics_api = lwe::GraphicsAPI::Unknown;
  if (system_info.vulkan_available) {
    LWE_LOG(Info, "Vulkan available, using Vulkan...");
    graphics_api = lwe::GraphicsAPI::Vulkan;
  } else {
    LWE_LOG(Info, "Vulkan not available, falling back to OpenGL...");
    graphics_api = lwe::GraphicsAPI::OpenGL;
  }

  //std::shared_ptr<lwe::IGraphicsSystem> graphics_system;
  //if (!lwe::IGraphicsSystem::Create(graphics_api, graphics_system)) {
  //  LWE_LOG(Fatal, "Could not initialize graphics subsystem.");
  //  return EXIT_FAILURE;
  //}

  //std::shared_ptr<lwe::IGraphicsDevice> device;
  //if (!graphics_system->CreateDevice(0, device)) {
  //  LWE_LOG(Fatal, "Failed to create graphics device.");
  //  return EXIT_FAILURE;
  //}

  //std::shared_ptr<lwe::IRenderBackend> render_backend;
  //if (!lwe::IRenderBackend::Create(device, render_backend)) {
  //  LWE_LOG(Fatal, "Failed to create render backend.");
  //  return EXIT_FAILURE;
  //}

#if 0
  //a basic set up...
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(640, 480);

  //create the window, the argument is the title
  glutCreateWindow("GLUT program");

  //pass the callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);

  glutMainLoop();
#endif
  // CGLPixelFormatAttribute attr[] = {
  //   kCGLPFATripleBuffer,
  //   static_cast<CGLPixelFormatAttribute>(0),
  // };
  // CGLPixelFormatObj pix[16]{};
  // GLint pix_count = 16;
  // CGLError error = CGLChoosePixelFormat(attr, pix, &pix_count);
  // if (error != kCGLNoError) {
  //   LWE_LOG(Error, "Failed to choose pixel format.");
  // } else {
  //   for (int32_t i = 0; i < pix_count; ++i) {
  //     LWE_LOG(Info, "Pixel Format: %d (%p)", i, pix[i]);
  //     CGLDestroyPixelFormat(pix[i]);
  //   }
  // }

  // std::shared_ptr<lwe::IGraphicsDevice> null_device;
  // std::shared_ptr<lwe::IRenderBackend> backend;
  // lwe::IRenderBackend::Create(null_device, backend);

  return EXIT_SUCCESS;
}
