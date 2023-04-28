#include "Rendering/GLFW_Window.hpp"
#include "Rendering/Context.hpp"
// #include "Wrapper/Base.hpp"
// #include "Wrapper/Instance.hpp"
// #include "wglext.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <vulkan/vulkan.hpp>

namespace MCRT {
Window::Window(int width, int height) {

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  // glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
  // // glfwSwapInterval(0);
    m_window = glfwCreateWindow(width, height, "MoChengRT", nullptr,
    nullptr);
//   window.reset(glfwCreateWindow(width, height, "MoChengRT", nullptr, nullptr));
//   glfwMakeContextCurrent(window);
  // glfwSwapInterval(0);
  assert(m_window);
}

Window::~Window() {

  glfwDestroyWindow(m_window);
  window.reset();
  glfwTerminate();
}
void Window::PollEvents() {
  glfwPollEvents();
  WindowUpdate();
}
bool Window::Should_Close() { return glfwWindowShouldClose(m_window); }
std::vector<int> Window::WindowUpdate() {
  std::vector<int> res(4);
  int stride = 1;
  int weight = 2;
  if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) {
    res[0] = stride * weight * -1;
  }

  if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    res[1] = stride * weight;
  }

  if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    res[2] = stride * weight * -1;
  }

  if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    res[3] = stride * weight;
  }
  return res;
}
void Window::CreateWindowSurface() {
  // auto& instance =
  // Context::Get_Singleton()->Get_Instance();

  // VK_CHECK_SUCCESS(glfwCreateWindowSurface(instance->Get_handle(),
  // window,
  //                      nullptr,
  //                      (VkSurfaceKHR*)(&surface)),
  // "Error: failed to create
  // surface");
}
} // namespace MCRT