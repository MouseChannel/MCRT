#include "example/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"

namespace MCRT {
void App::init() {
  window.reset(new Window(800, 600));
  Context::Get_Singleton()->init(window);
}
void App::run() {
  while (!window->Should_Close()) {
    window->PollEvents();
    //   window.s
  }
}
} // namespace MCRT