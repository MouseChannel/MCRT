#include <iostream>
#include <vulkan/vulkan.hpp>

// #include ""
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void*)
{
    std::cout << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
namespace MCRT {

}
// PFN_vkGetAccelerationStructureBuildSizesKHR pfnGetAccelerationStructureBuildSizesKHR;
// pfnGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));