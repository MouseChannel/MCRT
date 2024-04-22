#include "Helper/Link_Util.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include <vulkan/vulkan.hpp>

VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectNameEXT(
    VkDevice device,
    const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
{
    auto instance = MCRT::Context::Get_Singleton()->get_instance()->get_handle();
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
    auto func = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetAccelerationStructureBuildSizesKHR");
    return func(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
    auto func = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkCmdBuildAccelerationStructuresKHR");
    return func(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
    VkDevice device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkAccelerationStructureKHR* pAccelerationStructure)
{
    auto func = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkCreateAccelerationStructureKHR");
    return func(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetAccelerationStructureDeviceAddressKHR(
    VkDevice device,
    const VkAccelerationStructureDeviceAddressInfoKHR* pInfo)
{
    auto func = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetAccelerationStructureDeviceAddressKHR");
    return func(device, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines)
{
    auto func = (PFN_vkCreateRayTracingPipelinesKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkCreateRayTracingPipelinesKHR");
    return func(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData)
{
    auto func = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()
                                                                              ->get_device()
                                                                              ->get_handle(),
                                                                              "vkGetRayTracingShaderGroupHandlesKHR");
    return func(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    auto func = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()
                                                           ->get_device()
                                                           ->get_handle(),
                                                           "vkCmdTraceRaysKHR");
    return func(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
}

// static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
//     VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//     VkDebugUtilsMessageTypeFlagsEXT messageType,
//     VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
//     void*)
// {
//     std::cout << pCallbackData->pMessage << std::endl;

//     return VK_FALSE;
// }
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR void VKAPI_CALL vkGetQueueCheckpointData2NV(
    VkQueue queue,
    uint32_t* pCheckpointDataCount,
    VkCheckpointData2NV* pCheckpointData)
{

    auto func = (PFN_vkGetQueueCheckpointData2NV)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetQueueCheckpointData2NV");

    if (func != nullptr) {
        func(queue, pCheckpointDataCount, pCheckpointData);
    } else {
        throw std::runtime_error("fail get point");
    }
}

VKAPI_ATTR void VKAPI_CALL vkGetQueueCheckpointDataNV(
    VkQueue queue,
    uint32_t* pCheckpointDataCount,
    VkCheckpointDataNV* pCheckpointData)
{

    auto func = (PFN_vkGetQueueCheckpointDataNV)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetQueueCheckpointDataNV");

    if (func != nullptr) {
        func(queue, pCheckpointDataCount, pCheckpointData);
    } else {
        throw std::runtime_error("fail get point");
    }
}

// VKAPI_ATTR void VKAPI_CALL vkCmdSetCheckpointNV(
//     VkCommandBuffer commandBuffer,
//     const void* pCheckpointMarker)
// {
//     auto func = (PFN_vkCmdSetCheckpointNV)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkCmdSetCheckpointNV");

//     if (func != nullptr) {
//         func(commandBuffer, pCheckpointMarker);
//     } else {
//         throw std::runtime_error("fail set point");
//     }
// }

// VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceFaultInfoEXT(
//     VkDevice device,
//     VkDeviceFaultCountsEXT* pFaultCounts,
//     VkDeviceFaultInfoEXT* pFaultInfo)
// {
//     auto func = (PFN_vkGetDeviceFaultInfoEXT)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetDeviceFaultInfoEXT");
//     func = pfn_vkGetDeviceFaultInfoEXT;
//     if (func != nullptr) {
//         std::cout << "in" << std::endl;
//         func(device, pFaultCounts, pFaultInfo);
//     } else {
//         throw std::runtime_error("fail set point");
//     }
// }

VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreFdKHR(
    VkDevice device,
    const VkSemaphoreGetFdInfoKHR* pGetFdInfo,
    int* pFd)
{
    auto func = (PFN_vkGetSemaphoreFdKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetSemaphoreFdKHR");

    if (func != nullptr) {
        std::cout << "in" << std::endl;
        return func(device, pGetFdInfo, pFd);
    } else {
        throw std::runtime_error("fail set vkGetSemaphoreFdKHR");
    }
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdKHR(
    VkDevice device,
    const VkMemoryGetFdInfoKHR* pGetFdInfo,
    int* pFd)
{

    auto func = (PFN_vkGetMemoryFdKHR)vkGetDeviceProcAddr(MCRT::Context::Get_Singleton()->get_device()->get_handle(), "vkGetMemoryFdKHR");

    if (func != nullptr) {
        std::cout << "in" << std::endl;
        return func(device, pGetFdInfo, pFd);
    } else {
        throw std::runtime_error("fail set vkGetMemoryFdKHR");
    }
}
