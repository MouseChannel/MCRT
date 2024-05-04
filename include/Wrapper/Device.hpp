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
//                "VK_EXT_scalar_block_layout"
        //"VK_KHR_maintenance4"
        "VK_KHR_shader_non_semantic_info",

    };
    const std::vector<const char*> rt_device_extension {
        "VK_KHR_swapchain",

        "VK_KHR_ray_query",
        "VK_KHR_shader_non_semantic_info",

        "VK_KHR_acceleration_structure",
        "VK_KHR_ray_tracing_pipeline",
        "VK_KHR_shader_clock",
        // 'VK_KHR_buffer_device_address',
        "VK_KHR_deferred_host_operations",
        //  VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        "VK_KHR_external_semaphore_fd",
        "VK_KHR_external_memory_fd"

    };

    [[nodiscard("missing physical device")]] auto Get_Physical_device()
    {
        return physical_device;
    }

    [[nodiscard("Missing graphic_queue")]] auto Get_Graphic_queue()
    {
        return graphic_queue;
    }
    [[nodiscard("Missing graphic_queue")]] auto Get_Compute_queue()
    {
        return compute_queue;
    }

    [[nodiscard("Missing present_queue")]] auto Get_present_queue()
    {
        return present_queue;
    }

    [[nodiscard("missing queue_family_index")]] auto get_queuefamily_index()
    {
        return queue_family_indices;
    }

    auto get_deviceUUID()
    {
        return m_deviceUUID;
    }

    [[nodiscard]] vk::SampleCountFlagBits Get_sampler_count();
    [[nodiscard]] vk::Format Get_supported_format(std::vector<vk::Format> candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits feature);

    struct QueueFamilyIndices final {
        std::optional<uint32_t> graphic_queue;
        std::optional<uint32_t> present_queue;
        std::optional<uint32_t> compute_queue;

        bool Complete()
        {
            return graphic_queue.has_value() && present_queue.has_value() && compute_queue.has_value();
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
    vk::Queue compute_queue;
    vk::SampleCountFlagBits sampler_count;
    vk::FormatProperties supported_formats;
    uint8_t m_deviceUUID[VK_UUID_SIZE];
};
} // namespace MoCheng3D
