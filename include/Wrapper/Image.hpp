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
    Image(uint32_t width,
          uint32_t height,
          vk::Format format,
          vk::ImageType type,
          vk::ImageTiling tiling,
          vk::ImageUsageFlags usage,
          vk::ImageAspectFlags aspect,
          vk::SampleCountFlagBits sample);
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
    Image(vk::Image other_image,
          vk::ImageLayout image_layout,
          vk::Format format,
          vk::ImageAspectFlags aspect);
    Image(
        vk::Format format,
        vk::Image other_image,
        size_t width,
        size_t height);
    // only for cubemap
    Image(vk::Image other_image,
          vk::Format format,
          size_t width,
          size_t height);
    ~Image();
    void Create_ImageView(vk::Format format);

    void SetImageLayout(vk::ImageLayout dst_layout,
                        vk::AccessFlags src_access_mask,
                        vk::AccessFlags dst_access_mask,
                        vk::PipelineStageFlags src_stage_mask,
                        vk::PipelineStageFlags dst_stage_mask);
    void FillImageData(size_t size, void* data);
    void FillImageData(std::vector<size_t> size, std::vector<void*> data);
    void generate_mipmap( );
    [[nodiscard("Missing ImageView")]] auto& Get_Image_View()
    {
        return image_view;
    }
    [[nodiscard("missing image_layout")]] auto& Get_image_layout()
    {
        return image_layout;
    }

    uint32_t width, height;
    vk::ImageView image_view;
private:
    uint32_t layer_count = 1, mipmap_level_count = 1;
    vk::DeviceMemory memory;
    vk::ImageLayout image_layout { vk::ImageLayout::eUndefined };
    vk::ImageAspectFlags m_aspect;
    bool need_delete;
    void AllocateMemory();
    uint32_t FindMemoryTypeIndex(uint32_t requirement_type,
                                 vk::MemoryPropertyFlags flag);
};
} // namespace MoCheng3D