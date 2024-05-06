#include "Wrapper/SwapChain.hpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Window_Surface.hpp"
#include <array>
#include <cstdint>
#include <limits>
#include <new>
#include <set>
#include <vector>
#include <vulkan/vulkan_structs.hpp>

namespace MCRT {
vk::SwapchainKHR SwapChain::old_swapchian = VK_NULL_HANDLE;

SwapChain::SwapChain()
{
    Query_info();
    auto surface = Get_Context_Singleton()->get_surface();
    auto graphic_queue_index = Get_Context_Singleton()->get_device()->queue_family_indices.graphic_queue.value();

    auto present_queue_index = Get_Context_Singleton()->get_device()->queue_family_indices.present_queue.value();

    auto share_mode = graphic_queue_index == present_queue_index
        ? vk::SharingMode::eExclusive
        : vk::SharingMode::eConcurrent;

    std::set<uint32_t> queue_family_index { graphic_queue_index, present_queue_index };

    std::vector<uint32_t> queue_family_index_v;
    queue_family_index_v.assign(queue_family_index.begin(), queue_family_index.end());
    vk::SwapchainCreateInfoKHR createInfo;

    createInfo.setClipped(true)
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eInherit)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
#else
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage)
#endif
        .setImageExtent(surfaceInfo.extent)
        .setImageColorSpace(surfaceInfo.format.colorSpace)
        .setImageFormat(surfaceInfo.format.format)

        .setMinImageCount(surfaceInfo.count)
        .setImageArrayLayers(1)
        .setPresentMode(vk::PresentModeKHR::eFifo)
        .setPreTransform(surfaceInfo.transform)
        .setSurface(surface->get_handle())
        .setQueueFamilyIndices(queue_family_index_v)
        .setImageSharingMode(share_mode)
        .setOldSwapchain(old_swapchian);

    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createSwapchainKHR(createInfo);
    old_swapchian = m_handle;
    setImages();
}

SwapChain::~SwapChain()
{

    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroySwapchainKHR(m_handle);
}

void SwapChain::Query_info()
{
    surfaceInfo.format = Query_surface_Format();
    auto surface = Get_Context_Singleton()->get_surface();
    auto capability = Get_Context_Singleton()
                          ->get_device()
                          ->Get_Physical_device()
                          .getSurfaceCapabilitiesKHR(surface->get_handle());

    surfaceInfo.count =

        std::clamp(capability.minImageCount + 1,
                   capability.minImageCount,
                   capability.maxImageCount);
    surfaceInfo.count = capability.minImageCount + 1;
    // todo
    // surfaceInfo.count = 3;
    surfaceInfo.transform = capability.currentTransform;
    int width = 0, height = 0;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    surfaceInfo.extent = capability.currentExtent;
#else

    glfwGetFramebufferSize(
        Context::Get_Singleton()->get_window()->get_handle(),
        &width,
        &height);

    surfaceInfo.extent = Query_surface_Extent(capability, width, height);
#endif
    int e = 0;
}

vk::Extent2D
SwapChain::Query_surface_Extent(const vk::SurfaceCapabilitiesKHR& capability,
                                int windowWidth,
                                int windowHeight)
{
    if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capability.currentExtent;
    } else {
        auto extent = vk::Extent2D { static_cast<uint32_t>(windowWidth),
                                     static_cast<uint32_t>(windowHeight) };

        extent.width = std::clamp(extent.width,
                                  capability.minImageExtent.width,
                                  capability.maxImageExtent.width);
        extent.height = std::clamp(extent.height,
                                   capability.minImageExtent.height,
                                   capability.maxImageExtent.height);
        return extent;
    }
}

vk::SurfaceFormatKHR SwapChain::Query_surface_Format()
{
    auto physical_device = Get_Context_Singleton()->get_device()->Get_Physical_device();
    auto surface = Get_Context_Singleton()->get_surface();
    auto available_format = physical_device.getSurfaceFormatsKHR(surface->get_handle());
    for (auto& format : available_format) {
        if (format.format == vk::Format::eR8G8B8A8Unorm &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }
    return available_format[0];
}
void SwapChain::setImages()
{
    auto& image_handles = Get_Swapchain_ImagesHandle();
    images.resize(image_handles.size());

    std::transform(image_handles.begin(),
                   image_handles.end(),
                   images.begin(),
                   [&](auto& handle) { return std::shared_ptr<Image> {
                                           new Image(handle,
                                                     vk::ImageLayout::eColorAttachmentOptimal,
                                                     Get_Format(),
                                                     vk::ImageAspectFlagBits::eColor)
                                       }; }

    );
}

} // namespace MoCheng3D
