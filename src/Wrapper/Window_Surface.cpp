#include "Wrapper/Window_Surface.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Wrapper/Instance.hpp"

namespace MCRT {

Surface::Surface()
{

    auto vk_instance = Get_Context_Singleton()->get_instance();
    auto glfw_window = Get_Context_Singleton()->get_window();

    auto res = glfwCreateWindowSurface(vk_instance->get_handle(),
                                       glfw_window->get_window(),
                                       nullptr,
                                       (VkSurfaceKHR*)(&m_handle));
    if (res != VK_SUCCESS) {
        throw std::runtime_error("failed to create window_surface");
    }
}
Surface::~Surface()
{
    Get_Context_Singleton()->get_instance()->get_handle().destroySurfaceKHR(
        m_handle);
}
} // namespace MCRT