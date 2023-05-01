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
    Image(vk::Image other_image,
          vk::ImageLayout image_layout,
          vk::Format format,
          vk::ImageAspectFlags aspect);
    ~Image();
    void Create_ImageView(vk::Format format);

    void SetImageLayout(vk::ImageLayout dst_layout,
                        vk::AccessFlags src_access_mask,
                        vk::AccessFlags dst_access_mask,
                        vk::PipelineStageFlags src_stage_mask,
                        vk::PipelineStageFlags dst_stage_mask);
    void FillImageData(size_t size, void* data);
    [[nodiscard("Missing ImageView")]] auto& Get_Image_View()
    {
        return image_view;
    }
    [[nodiscard("missing image_layout")]] auto& Get_image_layout()
    {
        return image_layout;
    }

private:
    uint32_t width, height;
    vk::ImageView image_view;
    vk::DeviceMemory memory;
    vk::ImageLayout image_layout { vk::ImageLayout::eUndefined };
    vk::ImageAspectFlags m_aspect;
    bool need_delete;
    void AllocateMemory();
    uint32_t FindMemoryTypeIndex(std::uint32_t requirement_type,
                                 vk::MemoryPropertyFlags flag);
};
} // namespace MoCheng3D