#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Window {

private:
    std::shared_ptr<GLFWwindow> window;
    GLFWwindow* m_window;

public:
    [[nodiscard("Missing window")]] auto& get_handle()
    {
        return m_window;
    }
    Window(int width, int height);
    ~Window();
    void PollEvents();
    std::vector<int> WindowUpdate();
    bool Should_Close();
    void CreateWindowSurface();
};
} // namespace MCRT