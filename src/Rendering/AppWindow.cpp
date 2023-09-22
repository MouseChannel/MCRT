#include "Rendering/AppWindow.hpp"
#include "Rendering/Context.hpp"
// #include "Wrapper/Base.hpp"
// #include "Wrapper/Instance.hpp"
#include "Wrapper/Device.hpp"

#include <cstddef>
#include <vulkan/vulkan.hpp>

#if defined(VK_USE_PLATFORM_ANDROID_KHR)

#include "android_native_app_glue.h"

#else
#include <GLFW/glfw3.h>
#endif
namespace MCRT {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)

    Window::Window(ANativeWindow *android_window) {
        m_window = android_window;
    }

#else
    Window::Window(int width, int height)
    {

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
        // std::cout << Context::Get_Singleton()->get_device()->gpu_name << std::endl;
        // auto rr = Context::Get_Singleton()->get_device()->gpu_name;
        m_window = glfwCreateWindow(width, height, "MOCHENG", nullptr, nullptr);
        glfwSwapInterval(0);
        glfwSetWindowTitle(m_window, "new title");
        //   window.reset(glfwCreateWindow(width, height, "MoChengRT", nullptr, nullptr));
        //   glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        assert(m_window);
        int w = 0, h = 0;
        glfwGetFramebufferSize(m_window,
                               &w,
                               &h);
        Context::Get_Singleton()->set_extent2d(w, h);
    }

#endif

    Window::~Window() {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
        glfwDestroyWindow(m_window);

        glfwTerminate();
#endif
    }
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
    void Window::PollEvents() {

        glfwPollEvents();

        WindowUpdate();
    }

    bool Window::Should_Close() {

        return glfwWindowShouldClose(m_window);

    }

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
#endif
} // namespace MCRT
