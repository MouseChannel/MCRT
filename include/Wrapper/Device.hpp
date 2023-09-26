#pragma once

#include "Wrapper/Component.hpp"

#include <memory>
#include <optional>
#include <sys/types.h>

namespace MCRT {
class Device : public Component<vk::Device, Device> {

public:
    Device();
    ~Device();
    const std::vector<const char*> device_extension {
        "VK_KHR_swapchain",
//        "VK_KHR_shader_non_semantic_info"
//"VK_KHR_maintenance4"

    };
    const std::vector<const char*> rt_device_extension {
        "VK_KHR_swapchain",

//        "VK_KHR_ray_query",
//        "VK_KHR_shader_non_semantic_info",
//
//        "VK_KHR_acceleration_structure",
//        "VK_KHR_ray_tracing_pipeline",
//        "VK_KHR_shader_clock",
//        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
//        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
//        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,

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
    char* gpu_name;

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