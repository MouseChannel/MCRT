#include "Wrapper/Window_Surface.hpp"
#include "Rendering/AppWindow.hpp"
#include "Wrapper/Instance.hpp"
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include "vulkan/vulkan_android.h"
#endif
namespace MCRT {

Surface::Surface()
{

    auto vk_instance = Get_Context_Singleton()->get_instance();
    
    
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(vkGetInstanceProcAddr(
        vk_instance->get_handle(), "vkCreateAndroidSurfaceKHR"));
    vk::AndroidSurfaceCreateInfoKHR android_surface_ci = vk::AndroidSurfaceCreateInfoKHR()
                                                             .setWindow(Get_Context_Singleton()->get_window()->get_handle());
    
    vkCreateAndroidSurfaceKHR(vk_instance->get_handle(), (VkAndroidSurfaceCreateInfoKHR*)&android_surface_ci,
                              nullptr, (VkSurfaceKHR*)&m_handle);
#else
//    auto glfw_window = Get_Context_Singleton()->Get_Window();
//    VK_CHECK_SUCCESS(glfwCreateWindowSurface(vk_instance->Get_handle(),
//                                             glfw_window->get_window(), nullptr,
//                                             (VkSurfaceKHR*)(&m_handle)),
//                     "Error: failed to create surface");
    auto glfw_window = Get_Context_Singleton()->get_window();
    auto res = glfwCreateWindowSurface(vk_instance->get_handle(),
                                       glfw_window->get_handle(),
                                       nullptr,
                                       (VkSurfaceKHR*)(&m_handle));
    if (res != VK_SUCCESS) {
        throw std::runtime_error("failed to create window_surface");
    }
#endif
    

}
Surface::~Surface()
{
    Get_Context_Singleton()->get_instance()->get_handle().destroySurfaceKHR(
        m_handle);
}
} // namespace MCRT