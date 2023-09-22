#pragma once

#define GLFW_INCLUDE_NONE

#include <memory>
#include <vulkan/vulkan.hpp>

#if defined(VK_USE_PLATFORM_ANDROID_KHR)

#include "vulkan/vulkan_android.h"

#else
#include <GLFW/glfw3.h>
#endif
namespace MCRT {
    class Window {

    private:
//    std::shared_ptr<GLFWwindow> window;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        ANativeWindow *m_window;
#else
        GLFWwindow* m_window;
#endif

    public:
        [[nodiscard("Missing window")]] auto &get_handle() {
            return m_window;
        }
        ~Window();
#if defined(VK_USE_PLATFORM_ANDROID_KHR)

        Window(ANativeWindow *android_window);

#else
        Window(int width, int height);




        void PollEvents();

        std::vector<int> WindowUpdate();

        bool Should_Close();

        void CreateWindowSurface();
#endif
    };
} // namespace MCRT