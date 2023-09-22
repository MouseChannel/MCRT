#include "Wrapper/Device.hpp"
#include "Rendering/Context.hpp"
//#include "Rendering/GLFW_Window.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Window_Surface.hpp"
#include <cassert>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace MoCheng3D {
Device::Device()
{

    auto& instance = Get_Context_Singleton()->Get_Instance();

    auto avalible_physical_device = instance->Get_handle().enumeratePhysicalDevices();
    physical_device = avalible_physical_device[0];
    // auto rr =
    // uint32_t count;
    // std::vector<VkExtensionProperties> temp;
    // vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
    //     nullptr);
    // temp.resize(count);
    // vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
    //     temp.data());
    std::cout << "device extension: " << std::endl;
    for (auto& i : physical_device.enumerateDeviceExtensionProperties()) {
        std::cout << i.extensionName << std::endl;
    }
    assert(physical_device);
    QueryQueueFamilyIndices();

    vk::DeviceQueueCreateInfo queue_create_info;

    float priorities = 1.0f;

    if (queue_family_indices.present_queue.value() == queue_family_indices.graphic_queue.value()) {
    }

    queue_create_info.setPQueuePriorities(&priorities)
        .setQueueCount(1)
        .setQueueFamilyIndex(queue_family_indices.graphic_queue.value());

    vk::DeviceCreateInfo create_info;
    create_info.setQueueCreateInfos(queue_create_info)
        .setPEnabledExtensionNames(deviceRequiredExtensions);
    m_handle = physical_device.createDevice(create_info);

    graphic_queue = m_handle.getQueue(queue_family_indices.graphic_queue.value(), 0);
    present_queue = m_handle.getQueue(queue_family_indices.present_queue.value(), 0);
}

Device::~Device()
{
    // Context::Get_Singleton()->Get_Instance()->Get_handle()

    m_handle.destroy();
}

void Device::QueryQueueFamilyIndices()
{

    auto properties = physical_device.getQueueFamilyProperties();

    auto surface = Get_Context_Singleton()->Get_Surface();
    for (int i = 0; i < properties.size(); i++) {
        auto property = properties[i];
        if (property.queueFlags | vk::QueueFlagBits::eGraphics) {
            queue_family_indices.graphic_queue = i;
        }
        if (physical_device.getSurfaceSupportKHR(i, surface->Get_handle())) {
            queue_family_indices.present_queue = i;
        }
        if (queue_family_indices.Complete()) {
            break;
        }
    }
}

vk::SampleCountFlagBits Device::Get_sampler_count()
{

    auto res = Get_Physical_device().getProperties();
    auto count = std::min(res.limits.framebufferColorSampleCounts,
        res.limits.framebufferDepthSampleCounts);

    for (auto i { VkSampleCountFlags(vk::SampleCountFlagBits::e64) };
         i != VkSampleCountFlags(vk::SampleCountFlagBits::e1); i >>= 1) {
        auto cur = vk::SampleCountFlagBits(i);
        if (count & cur) {
            return cur;
        }
    }
    throw std::runtime_error("no vk::sampler_count");
}
vk::Format Device::Get_supported_format(std::vector<vk::Format> candidates,
    vk::ImageTiling tiling,
    vk::FormatFeatureFlagBits feature)
{
    for (auto& format : candidates) {

        auto property = Get_Physical_device().getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (property.linearTilingFeatures & feature) == feature) {
            return format;
        }

        if (tiling == vk::ImageTiling::eOptimal && (property.optimalTilingFeatures & feature) == feature) {
            return format;
        }
    }
    throw std::runtime_error("no support_format");
}

} // namespace MoCheng3D