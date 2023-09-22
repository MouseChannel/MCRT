#include "Wrapper/Image.hpp"
#include "Helper/CommandManager.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Device.hpp"
namespace MoCheng3D {
Image::Image(uint32_t width, uint32_t height, vk::Format format,
    vk::ImageType type, vk::ImageTiling tiling,
    vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
    vk::SampleCountFlagBits sample)
    : width(width)
    , height(height)
    , m_aspect(aspect)
    , need_delete(true)
{

    auto& device = Get_Context_Singleton()->Get_Device();
    vk::ImageCreateInfo create_info;
    vk::Extent3D extent { width, height, 1 };
    create_info.setImageType(vk::ImageType::e2D)
        .setArrayLayers(1)
        .setMipLevels(1)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(format)
        .setUsage(usage)
        .setExtent(extent)
        .setTiling(tiling)
        .setSamples(sample);

    m_handle = device->Get_handle().createImage(create_info);
    AllocateMemory();
    Get_Context_Singleton()->Get_Device()->Get_handle().bindImageMemory(
        m_handle, memory, 0);
    Create_ImageView(format);
}

Image::Image(vk::Image other_image, vk::ImageLayout image_layout,
    vk::Format format, vk::ImageAspectFlags aspect)
    : image_layout(image_layout)
    , m_aspect(aspect)
    , need_delete(false)
{

    m_handle = other_image;
    Create_ImageView(format);
}

void Image::Create_ImageView(vk::Format format)
{
    vk::ImageSubresourceRange range;
    range.setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1)
        .setAspectMask(m_aspect);
    vk::ImageViewCreateInfo view_create_info;
    view_create_info.setImage(m_handle)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(format)
        .setSubresourceRange(range);
    image_view = Get_Context_Singleton()->Get_Device()->Get_handle().createImageView(
        view_create_info);
}

void Image::AllocateMemory()
{
    auto image_memory_requirement = Get_Context_Singleton()
                                        ->Get_Device()
                                        ->Get_handle()
                                        .getImageMemoryRequirements(m_handle);
    auto memory_index = FindMemoryTypeIndex(image_memory_requirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    vk::MemoryAllocateInfo allocate_info;
    allocate_info.setAllocationSize(image_memory_requirement.size)
        .setMemoryTypeIndex(memory_index);
    memory = Get_Context_Singleton()
                 ->Get_Device()
                 ->Get_handle()
                 .allocateMemory(allocate_info);
}
uint32_t Image::FindMemoryTypeIndex(std::uint32_t requirement_type,
    vk::MemoryPropertyFlags flag)
{
    auto memory_properties = Get_Context_Singleton()
                                 ->Get_Device()
                                 ->Get_Physical_device()
                                 .getMemoryProperties();
    for (int i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((1 << i) & requirement_type && memory_properties.memoryTypes[i].propertyFlags & flag) {
            return i;
        }
    }
    throw std::runtime_error("find memory_properties failed");
}

void Image::SetImageLayout(vk::ImageLayout dst_layout, vk::AccessFlags src_access_mask, vk::AccessFlags dst_access_mask, vk::PipelineStageFlags src_stage_mask, vk::PipelineStageFlags dst_stage_mask)
{
    auto graphic_queue = Get_Context_Singleton()->Get_Device()->Get_Graphic_queue();
    CommandManager::ExecuteCmd(graphic_queue, [&](auto cmd) {
        vk::ImageSubresourceRange range;
        range.setLayerCount(1)
            .setBaseArrayLayer(0)
            .setLevelCount(1)
            .setBaseMipLevel(0)
            .setAspectMask(m_aspect);

        vk::ImageMemoryBarrier barrier;
        barrier.setImage(m_handle)
            .setOldLayout(image_layout)
            .setNewLayout(dst_layout)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setSrcAccessMask(src_access_mask)
            .setDstAccessMask(dst_access_mask)
            .setSubresourceRange(range);

        cmd.pipelineBarrier(src_stage_mask, dst_stage_mask, {}, {}, nullptr,
            barrier);
        image_layout = dst_layout;
    });
}
void Image::FillImageData(size_t size, void* data)
{
    std::unique_ptr<Buffer> image_buffer {
        new Buffer(size, vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible)
    };

    image_buffer->Update(data, size);
    auto graphic_queue = Get_Context_Singleton()->Get_Device()->Get_Graphic_queue();
    CommandManager::ExecuteCmd(graphic_queue, [&](auto cmd_buffer) {
        vk::BufferImageCopy region;
        vk::ImageSubresourceLayers subsource;
        subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setMipLevel(0)
            .setLayerCount(1);
        region.setBufferImageHeight(0)
            .setBufferOffset(0)
            .setImageOffset(0)
            .setImageExtent({ width, height, 1 })
            .setBufferRowLength(0)
            .setImageSubresource(subsource);
        cmd_buffer.copyBufferToImage(image_buffer->Get_handle(), m_handle,
            vk::ImageLayout::eTransferDstOptimal,
            region);
    });
}
Image::~Image()
{
    auto& device = Get_Context_Singleton()->Get_Device()->Get_handle();
    device.destroyImageView(image_view);
    if (need_delete) {

        device.freeMemory(memory);
        device.destroyImage(m_handle);
    }
}

} // namespace MoCheng3D