#include "Rendering/Context.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include <vulkan/vulkan.hpp>
VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectNameEXT(
    VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
{
    auto instance = MCRT::Context::Get_Singleton()->get_instance()->Get_handle();
    auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
    if (func) {
        return func(device, pNameInfo);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureBuildSizesKHR(
    VkDevice device,
    VkAccelerationStructureBuildTypeKHR buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
    const uint32_t* pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo)
{
    auto func = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->Get_handle(), "vkGetAccelerationStructureBuildSizesKHR");
    return func(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
    auto func = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->Get_handle(), "vkCmdBuildAccelerationStructuresKHR");
    return func(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
    VkDevice device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkAccelerationStructureKHR* pAccelerationStructure)
{
    auto func = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->Get_handle(), "vkCreateAccelerationStructureKHR");
    return func(device, pCreateInfo, pAllocator, pAccelerationStructure);
}