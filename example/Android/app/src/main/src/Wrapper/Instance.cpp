#include "Wrapper/Instance.hpp"
//#include "GLFW/glfw3.h"
#include "Wrapper/Base.hpp"
#include "Wrapper/DebugUtil.hpp"
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_funcs.hpp>

namespace MoCheng3D {

    Instance::Instance() {
        print_available_layers();
        print_available_extensions();
        vk::InstanceCreateInfo create_info;
        vk::ApplicationInfo app_info;
        std::vector<const char *> vaild_layer = {"VK_LAYER_KHRONOS_validation"};

        app_info.setApiVersion(VK_API_VERSION_1_1);
//        auto extensions = GetRequiredExtensions();
        std::vector<const char *> extensions{"VK_KHR_surface", "VK_KHR_android_surface"};
        create_info
                .setPApplicationInfo(&app_info)

                .setPEnabledExtensionNames(extensions)
                .setPEnabledLayerNames(vaild_layer);
        m_handle = vk::createInstance(create_info);

        // Enable_vaildLayer();
    }

    std::vector<const char *> Instance::GetRequiredExtensions() {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        return std::vector<const char *>{};
#else

        uint32_t glfwExtensionCount = 0;

        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions,
                                             glfwExtensions + glfwExtensionCount);

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        return extensions;
#endif
    }

    void Instance::print_available_layers() {
        std::cout << "Available layers:" << std::endl;

        for (const auto &layer: vk::enumerateInstanceLayerProperties()) {
            // std::cout << layer.layerName << std::endl;
        }
        // vk::enumerateInstanceLayerProperties();
    }

    void Instance::print_available_extensions() {

        std::cout << "Available extensions:" << std::endl;

        for (const auto &extension: vk::enumerateInstanceExtensionProperties()) {
            std::cout << extension.extensionName << std::endl;
        }
    }

    void Instance::Enable_vaildLayer() {

        using severity_bit = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        using message_bit = vk::DebugUtilsMessageTypeFlagBitsEXT;
        vk::DebugUtilsMessengerCreateInfoEXT create_info;
        create_info
                .setMessageSeverity(severity_bit::eError | severity_bit::eWarning)
                .setMessageType(message_bit::eGeneral | message_bit::ePerformance |
                                message_bit::eValidation)
                .setPfnUserCallback(&debugCallBack);

        m_debugger = m_handle.createDebugUtilsMessengerEXT(create_info);
    }

    Instance::~Instance() {
        if (m_debugger)
            m_handle.destroyDebugUtilsMessengerEXT(m_debugger);
        m_handle.destroy();
    }
// "VK_LAYER_KHRONOS_validation"
} // namespace MoCheng3D