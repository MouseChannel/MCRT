#pragma once
// #include "Rendering/GLFW_Window.hpp"
#include "Wrapper/Component.hpp"
// #include "Wrapper/Instance.hpp"

#include <memory>
#include <optional>
#include <sys/types.h>

namespace MCRT {
class Device : public Component<vk::Device, Device> {

public:
    Device();
    ~Device();
    const std::vector<std::string> device_extension {
        "VK_KHR_swapchain",
        "VK_KHR_acceleration_structure",
        "VK_KHR_ray_tracing_pipeline",
        "VK_KHR_deferred_host_operations",
        // "VK_EXT_descriptor_indexing",
        // "VK_KHR_buffer_device_address",
        // "VK_KHR_deferred_host_operations",
        // "VK_KHR_get_physical_device_properties"
    };
    const std::vector<const char*> deviceRequiredExtensions {
        "VK_KHR_swapchain",

        "VK_KHR_ray_query",
        "VK_KHR_shader_non_semantic_info",

        "VK_KHR_acceleration_structure",
        "VK_KHR_ray_tracing_pipeline",
        "VK_KHR_shader_clock",
        // VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        // VK_EXT_DEVICE_FAULT_EXTENSION_NAME
    };
    [[nodiscard("missing physical device")]] auto Get_Physical_device()
    {
        return physical_device;
    }
    [[nodiscard("Missing graphic_queue")]] auto Get_Graphic_queue()
    {
        return graphic_queue;
    }
    [[nodiscard("Missing present_queue")]] auto Get_present_queue()
    {
        return present_queue;
    }
    [[nodiscard("missing queue_family_index")]] auto get_queuefamily_index()
    {
        return queue_family_indices;
    }
    [[nodiscard]] vk::SampleCountFlagBits Get_sampler_count();
    [[nodiscard]] vk::Format Get_supported_format(std::vector<vk::Format> candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits feature);
    struct QueueFamilyIndices final {
        std::optional<uint32_t> graphic_queue;
        std::optional<uint32_t> present_queue;
        bool Complete()
        {
            return graphic_queue.has_value() && present_queue.has_value();
        }
    } queue_family_indices;
    //   QueueFamilyIndices queue_family_indices;
private:
    void QueryQueueFamilyIndices();
    void get_feature();

    vk::PhysicalDevice physical_device;
    vk::Queue graphic_queue;
    vk::Queue present_queue;
    vk::SampleCountFlagBits sampler_count;
    vk::FormatProperties supported_formats;
};
} // namespace MoCheng3D