#include "Wrapper/Window_Surface.hpp"
//#include "Rendering/GLFW_Window.hpp"
#include "Wrapper/Instance.hpp"
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include "vulkan/vulkan_android.h"
#endif
namespace MoCheng3D {

Surface::Surface()
{

    auto vk_instance = Get_Context_Singleton()->Get_Instance();
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(vkGetInstanceProcAddr(
        vk_instance->Get_handle(), "vkCreateAndroidSurfaceKHR"));
    vk::AndroidSurfaceCreateInfoKHR android_surface_ci = vk::AndroidSurfaceCreateInfoKHR()
                                                             .setWindow(Get_Context_Singleton()->Get_Window());

    vkCreateAndroidSurfaceKHR(vk_instance->Get_handle(), (VkAndroidSurfaceCreateInfoKHR*)&android_surface_ci,
        nullptr, (VkSurfaceKHR*)&m_handle);
#else
    auto glfw_window = Get_Context_Singleton()->Get_Window();
    VK_CHECK_SUCCESS(glfwCreateWindowSurface(vk_instance->Get_handle(),
                         glfw_window->get_window(), nullptr,
                         (VkSurfaceKHR*)(&m_handle)),
        "Error: failed to create surface");
#endif
}
Surface::~Surface()
{
    Get_Context_Singleton()->Get_Instance()->Get_handle().destroySurfaceKHR(m_handle);
}
}