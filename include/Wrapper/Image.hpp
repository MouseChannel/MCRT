#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include <sys/types.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
// #include "vulkan/vulkan.hpp"
// #include <vulkan/vulkan_handles.hpp>

namespace MCRT {
class Image : public Component<vk::Image, Image> {
public:
    Image() = default;
    ~Image();

    Image(uint32_t width,
          uint32_t height,
          vk::Format format,
          vk::ImageType type,
          vk::ImageTiling tiling,
          vk::ImageUsageFlags usage,
          vk::ImageAspectFlags aspect,
          vk::SampleCountFlagBits sample,
          int mipmap_level = 1);

    /*
     * for cubemap image
     */
    Image(uint32_t width,
          uint32_t height,
          vk::Format format,
          vk::ImageType type,
          vk::ImageTiling tiling,
          vk::ImageUsageFlags usage,
          vk::ImageAspectFlags aspect,
          vk::SampleCountFlagBits sample,
          size_t layer_count,
          size_t mipmap_level_count,
          vk::ImageCreateFlags flag);
    // for swapchain
    Image(vk::Image other_image,
          vk::ImageLayout image_layout,
          vk::Format format,
          vk::ImageAspectFlags aspect);

    static std::shared_ptr<Image> Create3DImage(uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::Format format, vk::ImageUsageFlags usage);
    void Create_ImageView(vk::Format format, int base_miplevel = 0, int mipmap_level_count = 1);

    void SetImageLayout(vk::ImageLayout dst_layout,
                        vk::AccessFlags src_access_mask,
                        vk::AccessFlags dst_access_mask,
                        vk::PipelineStageFlags src_stage_mask,
                        vk::PipelineStageFlags dst_stage_mask);
    void FillImageData(size_t size, void* data);
    void FillImageData(std::vector<size_t> size, std::vector<void*> data);
    void generate_mipmap();
    [[nodiscard("Missing ImageView")]] auto& Get_Image_View(int mipmap_level_index = 0)
    {
        return m_image_views[mipmap_level_index];
    }
    [[nodiscard("missing image_layout")]] auto& Get_image_layout()
    {
        return image_layout;
    }
    auto& Get_image_format()
    {
        return m_format;
    }

    uint32_t width, height, depth = 1;
    void set_mipmap_level_index(int mipmap_level_index)
    {
        m_mipmap_level_index = mipmap_level_index;
    }

private:
    uint32_t layer_count = 1, mipmap_level_count = 1;
    vk::DeviceMemory memory;
    // vk::ImageView image_view;
    uint32_t m_mipmap_level_index = 0;

    std::vector<vk::ImageView> m_image_views;
    vk::ImageLayout image_layout { vk::ImageLayout::eUndefined };
    vk::ImageAspectFlags m_aspect;
    vk::ImageType image_type { vk::ImageType::e2D };
    vk::Format m_format;
    bool need_delete = true;
    void AllocateMemory();
    uint32_t FindMemoryTypeIndex(uint32_t requirement_type,
                                 vk::MemoryPropertyFlags flag);
};
} // namespace MoCheng3D