#include <stdio.h>
#include <system/graphics_system.h>
#include <rendering/renderer.h>

int main(int argc, char *argv[]) {
  printf("starting...\n");

  std::shared_ptr<GraphicsSystem> system;
  if (!GraphicsSystem::Create(system)) {
    printf("error\n");
  } else {
    printf("done\n");
  }
  return 0;
}
