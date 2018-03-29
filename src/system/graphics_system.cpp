#include <system/graphics_system.h>

namespace lwe {

bool GraphicsSystem::Create(std::shared_ptr<GraphicsSystem> &out_system) {
  auto system = std::make_shared<GraphicsSystem>();
  if (!system->Initialize()) {
    return false;
  }
  out_system = system;
  return true;
}

GraphicsSystem::~GraphicsSystem() {
}

bool GraphicsSystem::Initialize() {
  return true;
}

} // namespace lwe
