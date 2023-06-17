#pragma once
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "Wrapper/Device.hpp"
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

static PFN_vkGetDeviceFaultInfoEXT pfn_vkGetDeviceFaultInfoEXT = 0;
