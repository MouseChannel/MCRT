#include "Wrapper/Device.hpp"
#include "Helper/Link_Util.hpp"

// #include "src/Helper/Link_Util.cpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Window_Surface.hpp"
#include <cassert>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace MCRT {
Device::Device()
{

    auto& instance = Get_Context_Singleton()->get_instance();

    auto avalible_physical_device = instance->get_handle().enumeratePhysicalDevices();
    physical_device = avalible_physical_device[0];

    std::cout << "avalible_gpu" << std::endl;
    for (auto i : avalible_physical_device) {

        std::cout << i.getProperties().deviceName << std::endl;
    }
    std::cout << "use :" << physical_device.getProperties().deviceName << std::endl;
    gpu_name = physical_device.getProperties().deviceName;
    std::string window_name = gpu_name;
    //    glfwSetWindowTitle(Context::Get_Singleton()->get_window()->get_handle(), window_name.c_str());
    //    glfwSetWindowTitle(Context::Get_Singleton()->get_window()->get_handle(), physical_device.getProperties().deviceName);
    std::cout
        << "device extension: " << std::endl;
    auto deviceExtensionProperties = physical_device.enumerateDeviceExtensionProperties();
    std::vector<std::string> data;
    for (auto& i : physical_device.enumerateDeviceExtensionProperties()) {
        // std::cout << i.extensionName << std::endl;
        auto rr = i.extensionName;
        data.push_back(i.extensionName);
    }
    assert(physical_device);
    QueryQueueFamilyIndices();

    vk::DeviceQueueCreateInfo queue_create_info;

    float priorities = 1.0f;

    if (queue_family_indices.present_queue.value() ==
        queue_family_indices.graphic_queue.value()) {
    }

    queue_create_info.setPQueuePriorities(&priorities)
                     .setQueueCount(1)
                     .setQueueFamilyIndex(queue_family_indices.graphic_queue.value());

    get_feature();
    vk::DeviceCreateInfo new_create_info;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    vk::PhysicalDeviceMaintenance4Features aa;
    auto r = physical_device.getProperties();
    vk::StructureChain<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceMaintenance4Properties> s;
    //    auto rr =physical_device.getProperties2< vk::PhysicalDeviceMaintenance4Features > (  );
    vk::PhysicalDeviceMaintenance4Properties pros;
    vk::PhysicalDeviceProperties2 properties1 {};
    properties1.pNext = &pros;

    vk::PhysicalDeviceMaintenance4Properties pp;
    PFN_vkGetPhysicalDeviceProperties2 func = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(vkGetInstanceProcAddr(
        Context::Get_Singleton()->get_instance()->get_handle(),
        "vkGetPhysicalDeviceProperties2"));
    // physical_device.getProperties2KHR();
    //        VkPhysicalDeviceProperties2 p;
    //        func(physical_device, (VkPhysicalDeviceProperties2 *) &s.get());
    //
    //        aa.setMaintenance4(true);
    vk::PhysicalDeviceFeatures feature;
    feature.setFragmentStoresAndAtomics(true)
        .setVertexPipelineStoresAndAtomics(true);
    new_create_info
        //                    .setPNext(&feature)
        .setQueueCreateInfos(queue_create_info)
        //                .setPEnabledFeatures(&feature)
        .setPEnabledExtensionNames(device_extension);
#else
    {
        //get deviceuuid
        vk::PhysicalDeviceIDPropertiesKHR deviceID;
        vk::PhysicalDeviceProperties2 properties2;
        properties2.pNext = &deviceID;

        physical_device.getProperties2(&properties2);
        memcpy(m_deviceUUID, deviceID.deviceUUID,VK_UUID_SIZE);
    }

    auto feature = physical_device.getFeatures2<vk::PhysicalDeviceFeatures2,
                                                vk::PhysicalDeviceVulkan13Features,
                                                vk::PhysicalDeviceVulkan12Features,
                                                vk::PhysicalDeviceVulkan11Features,
                                                vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
                                                vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
                                                vk::PhysicalDeviceRayQueryFeaturesKHR,
                                                vk::PhysicalDeviceShaderClockFeaturesKHR,
                                                vk::PhysicalDeviceFaultFeaturesEXT>();

    new_create_info
        .setPNext(&feature.get())
        .setQueueCreateInfos(queue_create_info)
        .setPEnabledExtensionNames(rt_device_extension);

    //                    uint32_t major = VK_VERSION_MAJOR(api_v);
    //                    uint32_t minor = VK_VERSION_MINOR(api_v);
    //                    uint32_t patch = VK_VERSION_PATCH(api_v);

    int r = 0;

#endif
    //    physical_device.createDevice(&new_create_info,nullptr, &m_handle);
    m_handle = physical_device.createDevice(new_create_info);
    std::cout << "success create device" << std::endl;
    graphic_queue = m_handle.getQueue(queue_family_indices.graphic_queue.value(), 0);
    present_queue = m_handle.getQueue(queue_family_indices.present_queue.value(), 0);
}

void Device::get_feature()
{
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceVulkan12Features,
                       vk::PhysicalDeviceVulkan11Features>
        features;

    //    auto feature = physical_device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan11Features>();
    //    int t = 0;
}

Device::~Device()
{
    // Context::Get_Singleton()->Get_Instance()->Get_handle()

    m_handle.destroy();
}

void Device::QueryQueueFamilyIndices()
{

    auto properties = physical_device.getQueueFamilyProperties();

    auto surface = Get_Context_Singleton()->get_window_surface();
    for (int i = 0; i < properties.size(); i++) {
        auto property = properties[i];
        if (property.queueFlags | vk::QueueFlagBits::eGraphics) {
            queue_family_indices.graphic_queue = i;
        }
        if (physical_device.getSurfaceSupportKHR(i,
                                                 surface->get_handle())) {
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

    for (auto i{ VkSampleCountFlags(vk::SampleCountFlagBits::e64) };
         i != VkSampleCountFlags(vk::SampleCountFlagBits::e1);
         i >>= 1) {
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
        if (tiling == vk::ImageTiling::eLinear &&
            (property.linearTilingFeatures & feature) == feature) {
            return format;
        }

        if (tiling == vk::ImageTiling::eOptimal &&
            (property.optimalTilingFeatures & feature) == feature) {
            return format;
        }
    }
    throw std::runtime_error("no support_format");
}

} // namespace MoCheng3D
