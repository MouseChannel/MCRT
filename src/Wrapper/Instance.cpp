#include "Wrapper/Instance.hpp"
#include "Helper/Link_Util.hpp"
#include "glfw/glfw3.h"

namespace MCRT {

auto Instance::get_required_extension()
{
    uint32_t glfwExtensionCount = 0;

    const char** glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    // extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    //   extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    //
    // extensions.push_back(VK_EXT_DEBUG);
    return extensions;
}
Instance::Instance()
{
    vk::InstanceCreateInfo create_info;
    vk::ApplicationInfo app_info;
    app_info // .setApplicationVersion(VK_API_VERSION_1_3)
        .setApiVersion(VK_API_VERSION_1_3);
    std::vector<const char*> vaild_layer = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor",
        //  "VK_LAYER_KHRONOS_synchronization2"
    };

    vk::ValidationFeaturesEXT features {};
    // vk::Feate
    auto debug = vk::ValidationFeatureEnableEXT::eDebugPrintf;
    std::vector<vk::ValidationFeatureDisableEXT> disables {
        vk::ValidationFeatureDisableEXT::eObjectLifetimes,
        vk::ValidationFeatureDisableEXT::eCoreChecks,
        vk::ValidationFeatureDisableEXT::eThreadSafety,
        vk::ValidationFeatureDisableEXT::eApiParameters,
        vk::ValidationFeatureDisableEXT::eUniqueHandles
    };
    // auto dsiable = vk::ValidationFeatureDisableEXT::
    features.setEnabledValidationFeatures(debug);
    //   .setDisabledValidationFeatures(
    //   disables);
    std::cout << "instance extension" << std::endl;
    for (auto i : vk::enumerateInstanceExtensionProperties()) {
        std::cout << i.extensionName << std::endl;
    }

    auto glfw_extension = get_required_extension();

    create_info.setPApplicationInfo(&app_info)
        .setPEnabledLayerNames(vaild_layer)
        .setPEnabledExtensionNames(glfw_extension)
        .setPNext(&features);
    m_handle = vk::createInstance(create_info);
    //
    using severity_bit = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    using message_bit = vk::DebugUtilsMessageTypeFlagBitsEXT;
    vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
    debug_create_info
        .setMessageSeverity(severity_bit::eError | severity_bit::eWarning | severity_bit::eInfo)
        .setMessageType(message_bit::eGeneral | message_bit::ePerformance | message_bit::eValidation | message_bit::eDeviceAddressBinding)
        .setPfnUserCallback(&debugCallBack);

    // m_handle.createDebugUtilsMessengerEXT()
    m_debugger = m_handle.createDebugUtilsMessengerEXT(debug_create_info);
}

} // namespace MCRT