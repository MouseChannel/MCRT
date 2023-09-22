#include "Wrapper/SwapChain.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Window_Surface.hpp"
#include <array>
#include <cstdint>
#include <limits>
#include <new>
#include <set>
#include <vector>
#include <vulkan/vulkan_structs.hpp>

namespace MoCheng3D {
SwapChain::SwapChain()
{
    Query_info();
    vk::SwapchainCreateInfoKHR createInfo;
    auto surface = Get_Context_Singleton()->Get_Surface();
    auto graphic_queue_index = Get_Context_Singleton()->Get_Device()->queue_family_indices.graphic_queue.value();

    auto present_queue_index = Get_Context_Singleton()->Get_Device()->queue_family_indices.present_queue.value();

    auto share_mode = graphic_queue_index == present_queue_index ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;

    std::set<uint32_t> queue_family_index { graphic_queue_index, present_queue_index };

    std::vector<uint32_t> queue_family_index_v;
    queue_family_index_v.assign(queue_family_index.begin(), queue_family_index.end());

    createInfo.setClipped(true)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eInherit)
        .setImageExtent(surfaceInfo.extent)
        .setImageColorSpace(surfaceInfo.format.colorSpace)
        .setImageFormat(surfaceInfo.format.format)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setMinImageCount(surfaceInfo.count)
        .setImageArrayLayers(1)
        .setPresentMode(vk::PresentModeKHR::eFifo)
        .setPreTransform(surfaceInfo.transform)
        .setSurface(surface->Get_handle())
        .setQueueFamilyIndices(queue_family_index_v)
        .setImageSharingMode(share_mode);
    m_handle = Get_Context_Singleton()->Get_Device()->Get_handle().createSwapchainKHR(createInfo);
}
SwapChain::~SwapChain()
{
    Get_Context_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroySwapchainKHR(m_handle);
}

void SwapChain::Query_info()
{
    surfaceInfo.format = Query_surface_Format();
    auto surface = Get_Context_Singleton()->Get_Surface();
    auto capability = Get_Context_Singleton()->Get_Device()->Get_Physical_device().getSurfaceCapabilitiesKHR(surface->Get_handle());
    //   capability.maxImageCount = 3;

    //   capability.minImageCount = 1;

    surfaceInfo.count =

        std::clamp(capability.minImageCount + 1, capability.minImageCount, capability.maxImageCount);
    // todo
    // surfaceInfo.count = 3;
    surfaceInfo.transform = capability.currentTransform;
    surfaceInfo.extent =  capability.currentExtent;
//            Query_surface_Extent(capability, 800, 800);
}
vk::Extent2D SwapChain::Query_surface_Extent(const vk::SurfaceCapabilitiesKHR& capability, int windowWidth,
    int windowHeight)
{
    if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capability.currentExtent;
    } else {
        auto extent = vk::Extent2D { static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight) };

        extent.width = std::clamp(extent.width, capability.minImageExtent.width, capability.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capability.minImageExtent.height, capability.maxImageExtent.height);
        return extent;
    }
}
vk::SurfaceFormatKHR SwapChain::Query_surface_Format()
{
    auto physical_device = Get_Context_Singleton()->Get_Device()->Get_Physical_device();
    auto surface = Get_Context_Singleton()->Get_Surface();
    auto available_format = physical_device.getSurfaceFormatsKHR(surface->Get_handle());
    for (auto& format : available_format) {
        if (format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }
    return available_format[0];
}

} // namespace MoCheng3D