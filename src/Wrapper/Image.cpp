#include "Wrapper/Image.hpp"
#include "Context/Context.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Device.hpp"
#include <cmath>


namespace MCRT {

Image::Image(uint32_t width,
             uint32_t height,
             vk::Format format,
             vk::ImageType type,
             vk::ImageTiling tiling,
             vk::ImageUsageFlags usage,
             vk::ImageAspectFlags aspect,
             vk::SampleCountFlagBits sample,
             int mipmap_level)
    : width(width)
    , height(height)
    , m_aspect(aspect)
    , image_type(type)
    , m_format(format)
    , mipmap_level_count(mipmap_level)
    , need_delete(true)
{
    auto& device = Get_Context_Singleton()->get_device();
    vk::ImageCreateInfo create_info;
    vk::Extent3D extent { width, height, 1 };
    create_info.setImageType(type)
        .setArrayLayers(1)
        .setMipLevels(mipmap_level)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(format)
        .setUsage(usage)
        .setExtent(extent)
        .setTiling(tiling)
        .setSamples(sample);

    m_handle = device->get_handle().createImage(create_info);
    // Context::Get_Singleton()->get_debugger()->set_handle_name(m_handle, "A img");
    // std::cout << "create img" << m_handle << std::endl;

    AllocateMemory();
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .bindImageMemory(
            m_handle,
            memory,
            0);
    Create_ImageView(format, 0, VK_REMAINING_MIP_LEVELS);
}

// Image::Image(uint32_t width,
//              uint32_t height,
//              uint32_t depth,
//              vk::Format format,
//              vk::ImageType type,
//              vk::ImageTiling tiling,
//              vk::ImageUsageFlags usage,
//              vk::ImageAspectFlags aspect,
//              vk::SampleCountFlagBits sample)
//     : width(width)
//     , height(height)
//     , m_aspect(aspect)
//     , image_type(type)
//     , m_format(format)
//     , need_delete(true)
// {
//     auto& device = Get_Context_Singleton()->get_device();
//     vk::ImageCreateInfo create_info;
//     vk::Extent3D extent { width, height, depth };
//     create_info.setImageType(type)
//         .setArrayLayers(1)
//         .setMipLevels(1)
//         .setInitialLayout(vk::ImageLayout::eUndefined)
//         .setFormat(format)
//         .setUsage(usage)
//         .setExtent(extent)
//         .setTiling(tiling)
//         .setSamples(sample);

//     m_handle = device->get_handle().createImage(create_info);
//     AllocateMemory();
//     Get_Context_Singleton()
//         ->get_device()
//         ->get_handle()
//         .bindImageMemory(
//             m_handle,
//             memory,
//             0);
//     Create_ImageView(format);
// }

Image::Image(uint32_t width,
             uint32_t height,
             vk::Format format,
             vk::ImageType type,
             vk::ImageTiling tiling,
             vk::ImageUsageFlags usage,
             vk::ImageAspectFlags aspect,
             vk::SampleCountFlagBits sample,
             size_t layer_count,
             size_t mipmap_level_count,
             vk::ImageCreateFlags flag)
    : width(width)
    , height(height)
    , m_aspect(aspect)
    , m_format(format)
    , layer_count(layer_count)
    , mipmap_level_count(mipmap_level_count)
    , need_delete(true)
{
    auto& device = Get_Context_Singleton()->get_device();
    vk::ImageCreateInfo create_info;
    vk::Extent3D extent { width, height, 1 };
    create_info.setImageType(type)
        .setArrayLayers(layer_count)
        .setMipLevels(mipmap_level_count)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(format)
        .setUsage(usage)
        .setExtent(extent)
        .setTiling(tiling)
        .setSamples(sample)
        .setFlags(flag);

    m_handle = device->get_handle().createImage(create_info);
    // std::cout << "create img" << m_handle << std::endl;
    AllocateMemory();
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .bindImageMemory(
            m_handle,
            memory,
            0);
    Create_ImageView(format, 0, VK_REMAINING_MIP_LEVELS);
    for (int i = 1; i < mipmap_level_count; i++)
        Create_ImageView(format, i, 1);
}

Image::Image(vk::Image other_image,
             vk::ImageLayout image_layout,
             vk::Format format,
             vk::ImageAspectFlags aspect)
    : image_layout(image_layout)
    , m_aspect(aspect)
    , m_format(format)
    , need_delete(false)
{

    m_handle = other_image;
    Create_ImageView(format, 0, 1);
}

// Image::Image(
//     vk::Format format,
//     vk::Image other_image,
//     size_t width,
//     size_t height)
//     : image_layout(vk::ImageLayout::eUndefined)
//     , m_aspect(vk::ImageAspectFlagBits::eColor)
//     , width(width)
//     , height(height)
//     , need_delete(true)
// {
//
//     m_handle = other_image;
//     AllocateMemory();
//     Get_Context_Singleton()
//         ->get_device()
//         ->get_handle()
//         .bindImageMemory(
//             m_handle,
//             memory,
//             0);
//     Create_ImageView(format);
// }

// Image::Image(vk::Image other_image,
//              vk::Format format,
//              size_t width,
//              size_t height)
//     : image_layout(vk::ImageLayout::eUndefined)
//     , m_aspect(vk::ImageAspectFlagBits::eColor)
//     , width(width)
//     , height(height)
//     , need_delete(true)
// {
//
//     m_handle = other_image;
//     AllocateMemory();
//     Get_Context_Singleton()
//         ->get_device()
//         ->get_handle()
//         .bindImageMemory(
//             m_handle,
//             memory,
//             0);
//     Create_ImageView(format);
// }

void Image::Create_ImageView(vk::Format format, int base_miplevel, int mipmap_level_count)
{
    vk::ImageSubresourceRange range;
    range.setBaseMipLevel(base_miplevel)
        // .setLevelCount(auto_mipmap ? std::log2(std::min(width, height)) : 1)
        // .setLevelCount(base_miplevel == 0 ? VK_REMAINING_MIP_LEVELS : 1)

        .setLevelCount(mipmap_level_count)

        .setBaseArrayLayer(0)
        .setLayerCount(VK_REMAINING_ARRAY_LAYERS)
        .setAspectMask(m_aspect);
    vk::ImageViewCreateInfo view_create_info;
    auto view_type = vk::ImageViewType::e2D;
    if (layer_count > 1) {
        view_type = vk::ImageViewType::eCube;
    } else {
        if (image_type == vk::ImageType::e2D) {
            view_type = vk::ImageViewType::e2D;
        } else if (image_type == vk::ImageType::e3D) {
            view_type = vk::ImageViewType::e3D;
        }
    }
    view_create_info.setImage(m_handle)
        .setViewType(view_type)
        .setFormat(format)

        .setSubresourceRange(range);
    m_image_views.emplace_back(Get_Context_Singleton()
                                   ->get_device()
                                   ->get_handle()
                                   .createImageView(
                                       view_create_info));
    // image_view = Get_Context_Singleton()
    //                  ->get_device()
    //                  ->get_handle()
    //                  .createImageView(
    //                      view_create_info);
    if (layer_count == 6) {
        // Context::Get_Singleton()->get_debugger()->set_handle_name(image_view,
        //                                                           "skybox_image_view");
    }
}
// void Image::Create_ImageView(vk::Format format, int base_miplevel, bool auto_mipmap)
// {
//     vk::ImageSubresourceRange range;
//     range.setBaseMipLevel(base_miplevel)
//         .setLevelCount(10)
//         .setBaseArrayLayer(0)
//         .setLayerCount(layer_count)
//         .setAspectMask(m_aspect);
//     vk::ImageViewCreateInfo view_create_info;
//     auto view_type = vk::ImageViewType::e2D;
//     if (layer_count > 1) {
//         view_type = vk::ImageViewType::eCube;
//     } else {

//         if (image_type == vk::ImageType::e2D) {
//             view_type = vk::ImageViewType::e2D;
//         } else if (image_type == vk::ImageType::e3D) {
//             view_type = vk::ImageViewType::e3D;
//         }
//     }
//     view_create_info.setImage(m_handle)
//         .setViewType(view_type)
//         .setFormat(format)
//         .setSubresourceRange(range);
//     m_image_views.emplace_back(Get_Context_Singleton()
//                                    ->get_device()
//                                    ->get_handle()
//                                    .createImageView(
//                                        view_create_info));
//     // image_view = Get_Context_Singleton()
//     //                  ->get_device()
//     //                  ->get_handle()
//     //                  .createImageView(
//     //                      view_create_info);
//     if (layer_count == 6) {
//         // Context::Get_Singleton()->get_debugger()->set_handle_name(image_view,
//         //                                                           "skybox_image_view");
//     }
// }

void Image::AllocateMemory()
{
    auto image_memory_requirement = Get_Context_Singleton()
                                        ->get_device()
                                        ->get_handle()
                                        .getImageMemoryRequirements(m_handle);
    auto memory_index = FindMemoryTypeIndex(image_memory_requirement.memoryTypeBits,
                                            vk::MemoryPropertyFlagBits::eDeviceLocal);
    vk::MemoryAllocateInfo allocate_info;
    allocate_info.setAllocationSize(image_memory_requirement.size)
        .setMemoryTypeIndex(memory_index);
    memory = Get_Context_Singleton()
                 ->get_device()
                 ->get_handle()
                 .allocateMemory(allocate_info);
}

uint32_t Image::FindMemoryTypeIndex(uint32_t requirement_type,
                                    vk::MemoryPropertyFlags flag)
{
    auto memory_properties = Get_Context_Singleton()
                                 ->get_device()
                                 ->Get_Physical_device()
                                 .getMemoryProperties();
    for (int i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((1 << i) & requirement_type &&
            memory_properties.memoryTypes[i].propertyFlags & flag) {
            return i;
        }
    }
    throw std::runtime_error("find memory_properties failed");
}

std::shared_ptr<Image> Image::Create3DImage(uint32_t width, uint32_t height, uint32_t depth, vk::ImageType type, vk::Format format, vk::ImageUsageFlags usage)
{
    auto& device = Context::Get_Singleton()->get_device();
    vk::ImageCreateInfo create_info;
    vk::Extent3D extent { width, height, depth };
    create_info.setImageType(type)
        .setArrayLayers(1)
        .setMipLevels(1)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFormat(format)
        .setUsage(usage)
        .setExtent(extent)
        .setTiling(vk::ImageTiling::eOptimal)
        .setSamples(vk::SampleCountFlagBits::e1);

    std::shared_ptr<Image> res {};

    res->m_handle = device->get_handle().createImage(create_info);
    // m_handle = device->get_handle().createImage(create_info);
    res->AllocateMemory();
    Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .bindImageMemory(
            res->m_handle,
            res->memory,
            0);
    res->Create_ImageView(format, 0, 1);
    return res;
}
void Image::SetImageLayout(vk::ImageLayout dst_layout,
                           vk::AccessFlags src_access_mask,
                           vk::AccessFlags dst_access_mask,
                           vk::PipelineStageFlags src_stage_mask,
                           vk::PipelineStageFlags dst_stage_mask)
{
    auto graphic_queue = Get_Context_Singleton()
                             ->get_device()
                             ->Get_Graphic_queue();
    CommandManager::ExecuteCmd(graphic_queue, [&](auto cmd) {
        vk::ImageSubresourceRange range;
        range.setLayerCount(layer_count)
            .setBaseArrayLayer(0)
            .setLevelCount(mipmap_level_count)
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

        cmd.pipelineBarrier(src_stage_mask,
                            dst_stage_mask,
                            {},
                            {},
                            nullptr,
                            barrier);
        image_layout = dst_layout;
    });
}

void Image::FillImageData(size_t size, void* data)
{
    std::unique_ptr<Buffer> image_buffer {
        new Buffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible)
    };
    Context::Get_Singleton()->get_debugger()->set_handle_name(image_buffer->get_handle(), "temp img buffer");
    image_buffer->Update(data, size);
    auto graphic_queue = Get_Context_Singleton()
                             ->get_device()
                             ->Get_Graphic_queue();
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
        cmd_buffer.copyBufferToImage(image_buffer->get_handle(),
                                     m_handle,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     region);
    });
}

void Image::FillImageData(std::vector<size_t> size, std::vector<void*> data)
{
    size_t total_size = 0;
    for (auto i : size) {
        total_size += i;
    }
    std::unique_ptr<Buffer> image_buffer {
        new Buffer(total_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible)
    };
    Context::Get_Singleton()->get_debugger()->set_handle_name(image_buffer->get_handle(), "temp img buffer");
    image_buffer->Update(data, size);
    auto graphic_queue = Get_Context_Singleton()
                             ->get_device()
                             ->Get_Graphic_queue();
    CommandManager::ExecuteCmd(graphic_queue, [&](auto cmd_buffer) {
        std::vector<vk::BufferImageCopy> copys(data.size());
        size_t offset = 0;
        for (int i = 0; i < copys.size(); i++) {
            copys[i]
                .setBufferImageHeight(0)
                .setBufferOffset(offset)
                // .setImageOffset(0)
                .setImageExtent({ width, height, 1 })
                .setBufferRowLength(0)
                .setImageSubresource(vk::ImageSubresourceLayers()
                                         .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                         .setBaseArrayLayer(i)
                                         .setLayerCount(1)
                                         .setMipLevel(0));
            offset += size[i];
        }
        cmd_buffer.copyBufferToImage(image_buffer->get_handle(),
                                     m_handle,
                                     vk::ImageLayout::eTransferDstOptimal,
                                     copys);
    });
}

void Image::generate_mipmap()
{
    SetImageLayout(vk::ImageLayout::eTransferSrcOptimal,
                   vk::AccessFlagBits::eTransferWrite,
                   vk::AccessFlagBits::eTransferRead,
                   vk::PipelineStageFlagBits::eTransfer,
                   vk::PipelineStageFlagBits::eTransfer);
    int count = std::log2(std::min(width, height));

    for (int i = 1; i < mipmap_level_count; i++) {

        CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                                   [&](vk::CommandBuffer cmd) {
                                       auto pre_image_barrier = vk::ImageMemoryBarrier {}.setImage(
                                                                                             m_handle)
                                                                    .setOldLayout(vk::ImageLayout::eUndefined)
                                                                    .setNewLayout(
                                                                        vk::ImageLayout::eTransferDstOptimal)
                                                                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                                                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                                                    //                    .setSrcStageMask(vk::PipelineStageFlagBits::eTransfer)
                                                                    .setSrcAccessMask(vk::AccessFlagBits::eNone)
                                                                    //                    .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
                                                                    .setDstAccessMask(
                                                                        vk::AccessFlagBits::eTransferWrite)
                                                                    .setSubresourceRange(
                                                                        vk::ImageSubresourceRange()
                                                                            .setAspectMask(
                                                                                vk::ImageAspectFlagBits::eColor)
                                                                            .setLayerCount(layer_count)
                                                                            .setBaseArrayLayer(0)
                                                                            .setLevelCount(1)
                                                                            .setBaseMipLevel(i));
                                       auto post_image_barrier = vk::ImageMemoryBarrier {}.setImage(
                                                                                              m_handle)
                                                                     .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                                                                     .setNewLayout(
                                                                         vk::ImageLayout::eTransferSrcOptimal)
                                                                     .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                                                     .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                                                     //                    .setSrcStageMask(vk::PipelineStageFlagBits::eTransfer)
                                                                     .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                                                                     //                    .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
                                                                     .setDstAccessMask(
                                                                         vk::AccessFlagBits::eTransferRead)
                                                                     .setSubresourceRange(
                                                                         vk::ImageSubresourceRange()
                                                                             .setAspectMask(
                                                                                 vk::ImageAspectFlagBits::eColor)
                                                                             .setLayerCount(layer_count)
                                                                             .setBaseArrayLayer(0)
                                                                             .setLevelCount(1)
                                                                             .setBaseMipLevel(i));
                                       cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                           vk::PipelineStageFlagBits::eTransfer,
                                                           {},
                                                           {},
                                                           {},
                                                           pre_image_barrier);
                                       cmd.blitImage(m_handle,
                                                     vk::ImageLayout::eTransferSrcOptimal,
                                                     m_handle,
                                                     vk::ImageLayout::eTransferDstOptimal,
                                                     vk::ImageBlit {}
                                                         .setSrcOffsets(
                                                             { vk::Offset3D { 0, 0, 0 },
                                                               vk::Offset3D { (int)width >> (i - 1), (int)height >> (i - 1), 1 } })
                                                         .setDstOffsets({ vk::Offset3D { 0, 0, 0 }, vk::Offset3D { (int)width >> i, (int)height >> i, 1 } })
                                                         .setSrcSubresource(
                                                             vk::ImageSubresourceLayers()
                                                                 .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                                 .setBaseArrayLayer(0)
                                                                 .setLayerCount(layer_count)
                                                                 .setMipLevel(i - 1))
                                                         .setDstSubresource(
                                                             vk::ImageSubresourceLayers()
                                                                 .setAspectMask(
                                                                     vk::ImageAspectFlagBits::eColor)
                                                                 .setBaseArrayLayer(
                                                                     0)
                                                                 .setLayerCount(
                                                                     layer_count)
                                                                 .setMipLevel(
                                                                     i)),
                                                     vk::Filter::eLinear);
                                       cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                           vk::PipelineStageFlagBits::eTransfer,
                                                           {},
                                                           {},
                                                           {},
                                                           post_image_barrier);

                                       // #if defined(VK_USE_PLATFORM_ANDROID_KHR)
                                       //                                        auto image_barrier = vk::ImageMemoryBarrier {}.setImage(
                                       //                                                                                          m_handle)
                                       //                                                                 .setOldLayout(vk::ImageLayout::eUndefined)
                                       //                                                                 .setNewLayout(
                                       //                                                                     vk::ImageLayout::eTransferDstOptimal)
                                       //                                                                 .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                       //                                                                 .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                       //                                                                 //                    .setSrcStageMask(vk::PipelineStageFlagBits::eTransfer)
                                       //                                                                 .setSrcAccessMask(vk::AccessFlagBits::eNone)
                                       //                                                                 //                    .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
                                       //                                                                 .setDstAccessMask(
                                       //                                                                     vk::AccessFlagBits::eTransferWrite)
                                       //                                                                 .setSubresourceRange(
                                       //                                                                     vk::ImageSubresourceRange()
                                       //                                                                         .setAspectMask(
                                       //                                                                             vk::ImageAspectFlagBits::eColor)
                                       //                                                                         .setLayerCount(layer_count)
                                       //                                                                         .setBaseArrayLayer(0)
                                       //                                                                         .setLevelCount(1)
                                       //                                                                         .setBaseMipLevel(i));
                                       //                                        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                       //                                                            vk::PipelineStageFlagBits::eTransfer,
                                       //                                                            {},
                                       //                                                            {},
                                       //                                                            {},
                                       //                                                            image_barrier);
                                       //                                        cmd.blitImage(m_handle,
                                       //                                                      vk::ImageLayout::eTransferSrcOptimal,
                                       //                                                      m_handle,
                                       //                                                      vk::ImageLayout::eTransferDstOptimal,
                                       //                                                      vk::ImageBlit {}
                                       //                                                          .setSrcOffsets(
                                       //                                                              { vk::Offset3D { 0, 0, 0 },
                                       //                                                                vk::Offset3D { (int)width >> (i - 1), (int)height >> (i - 1), 1 } })
                                       //                                                          .setDstOffsets({ vk::Offset3D { 0, 0, 0 }, vk::Offset3D { (int)width >> i, (int)height >> i, 1 } })
                                       //                                                          .setSrcSubresource(
                                       //                                                              vk::ImageSubresourceLayers()
                                       //                                                                  .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                       //                                                                  .setBaseArrayLayer(0)
                                       //                                                                  .setLayerCount(layer_count)
                                       //                                                                  .setMipLevel(i - 1))
                                       //                                                          .setDstSubresource(
                                       //                                                              vk::ImageSubresourceLayers()
                                       //                                                                  .setAspectMask(
                                       //                                                                      vk::ImageAspectFlagBits::eColor)
                                       //                                                                  .setBaseArrayLayer(
                                       //                                                                      0)
                                       //                                                                  .setLayerCount(
                                       //                                                                      layer_count)
                                       //                                                                  .setMipLevel(
                                       //                                                                      i)),
                                       //                                                      vk::Filter::eLinear);
                                       // #else
                                       //             cmd.pipelineBarrier2(
                                       //                 vk::DependencyInfo()
                                       //                     .setImageMemoryBarriers(
                                       //                         vk::ImageMemoryBarrier2()
                                       //                             .setImage(m_handle)
                                       //                             .setOldLayout(vk::ImageLayout::eUndefined)
                                       //                             .setNewLayout(
                                       //                                 vk::ImageLayout::eTransferDstOptimal)
                                       //                             .setSrcQueueFamilyIndex(
                                       //                                 VK_QUEUE_FAMILY_IGNORED)
                                       //                             .setDstQueueFamilyIndex(
                                       //                                 VK_QUEUE_FAMILY_IGNORED)
                                       //                             .setSrcStageMask(
                                       //                                 vk::PipelineStageFlagBits2::eTransfer)
                                       //                             .setSrcAccessMask(vk::AccessFlagBits2::eNone)
                                       //                             .setDstStageMask(
                                       //                                 vk::PipelineStageFlagBits2::eTransfer)
                                       //                             .setDstAccessMask(
                                       //                                 vk::AccessFlagBits2::eTransferWrite)
                                       //                             .setSubresourceRange(
                                       //                                 vk::ImageSubresourceRange()
                                       //                                     .setAspectMask(
                                       //                                         vk::ImageAspectFlagBits::eColor)
                                       //                                     .setLayerCount(layer_count)
                                       //                                     .setBaseArrayLayer(0)
                                       //                                     .setLevelCount(1)
                                       //                                     .setBaseMipLevel(i))));
                                       // #endif
                                   });

        //         CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
        //                                    [&](vk::CommandBuffer cmd) {
        // #if defined(VK_USE_PLATFORM_ANDROID_KHR)
        //                                        cmd.blitImage(m_handle,
        //                                                      vk::ImageLayout::eTransferSrcOptimal,
        //                                                      m_handle,
        //                                                      vk::ImageLayout::eTransferDstOptimal,
        //                                                      vk::ImageBlit {}
        //                                                          .setSrcOffsets(
        //                                                              { vk::Offset3D { 0, 0, 0 },
        //                                                                vk::Offset3D { (int)width >> (i - 1), (int)height >> (i - 1), 1 } })
        //                                                          .setDstOffsets({ vk::Offset3D { 0, 0, 0 }, vk::Offset3D { (int)width >> i, (int)height >> i, 1 } })
        //                                                          .setSrcSubresource(
        //                                                              vk::ImageSubresourceLayers()
        //                                                                  .setAspectMask(vk::ImageAspectFlagBits::eColor)
        //                                                                  .setBaseArrayLayer(0)
        //                                                                  .setLayerCount(layer_count)
        //                                                                  .setMipLevel(i - 1))
        //                                                          .setDstSubresource(
        //                                                              vk::ImageSubresourceLayers()
        //                                                                  .setAspectMask(
        //                                                                      vk::ImageAspectFlagBits::eColor)
        //                                                                  .setBaseArrayLayer(
        //                                                                      0)
        //                                                                  .setLayerCount(
        //                                                                      layer_count)
        //                                                                  .setMipLevel(
        //                                                                      i)),
        //                                                      vk::Filter::eLinear);
        // #else
        //             cmd.blitImage2(vk::BlitImageInfo2()
        //                                .setSrcImage(m_handle)
        //                                .setSrcImageLayout(
        //                                    vk::ImageLayout::eTransferSrcOptimal)
        //                                .setDstImage(m_handle)
        //                                .setDstImageLayout(
        //                                    vk::ImageLayout::eTransferDstOptimal)
        //                                .setRegions(
        //                                    vk::ImageBlit2()
        //                                        .setSrcOffsets(
        //                                            { vk::Offset3D {
        //                                                  0,
        //                                                  0,
        //                                                  0 },
        //                                              vk::Offset3D {
        //                                                  (int)width >> (i - 1),
        //                                                  (int)height >> (i - 1),
        //                                                  1 } })
        //                                        .setDstOffsets(
        //                                            { vk::Offset3D {
        //                                                  0,
        //                                                  0,
        //                                                  0 },
        //                                              vk::Offset3D {
        //                                                  (int)width >> i,
        //                                                  (int)height >> i,
        //                                                  1 } })
        //                                        .setSrcSubresource(
        //                                            vk::ImageSubresourceLayers()
        //                                                .setAspectMask(
        //                                                    vk::ImageAspectFlagBits::eColor)
        //                                                .setBaseArrayLayer(
        //                                                    0)
        //                                                .setLayerCount(
        //                                                    layer_count)
        //                                                .setMipLevel(
        //                                                    i -
        //                                                    1))
        //                                        .setDstSubresource(
        //                                            vk::ImageSubresourceLayers()
        //                                                .setAspectMask(
        //                                                    vk::ImageAspectFlagBits::eColor)
        //                                                .setBaseArrayLayer(
        //                                                    0)
        //                                                .setLayerCount(
        //                                                    layer_count)
        //                                                .setMipLevel(
        //                                                    i)))
        //                                .setFilter(vk::Filter::eLinear));

        // #endif
        //                                    });
        //         CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
        //                                    [&](vk::CommandBuffer cmd) {
        // #if defined(VK_USE_PLATFORM_ANDROID_KHR)
        //                                        auto image_barrier = vk::ImageMemoryBarrier {}.setImage(
        //                                                                                          m_handle)
        //                                                                 .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        //                                                                 .setNewLayout(
        //                                                                     vk::ImageLayout::eTransferSrcOptimal)
        //                                                                 .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        //                                                                 .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        //                                                                 //                    .setSrcStageMask(vk::PipelineStageFlagBits::eTransfer)
        //                                                                 .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        //                                                                 //                    .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
        //                                                                 .setDstAccessMask(
        //                                                                     vk::AccessFlagBits::eTransferRead)
        //                                                                 .setSubresourceRange(
        //                                                                     vk::ImageSubresourceRange()
        //                                                                         .setAspectMask(
        //                                                                             vk::ImageAspectFlagBits::eColor)
        //                                                                         .setLayerCount(layer_count)
        //                                                                         .setBaseArrayLayer(0)
        //                                                                         .setLevelCount(1)
        //                                                                         .setBaseMipLevel(i));
        //                                        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
        //                                                            vk::PipelineStageFlagBits::eTransfer,
        //                                                            {},
        //                                                            {},
        //                                                            {},
        //                                                            image_barrier);
        // #else
        //             cmd.pipelineBarrier2(
        //                 vk::DependencyInfo()
        //                     .setImageMemoryBarriers(
        //                         vk::ImageMemoryBarrier2()
        //                             .setImage(m_handle)
        //                             .setOldLayout(
        //                                 vk::ImageLayout::eTransferDstOptimal)
        //                             .setNewLayout(
        //                                 vk::ImageLayout::eTransferSrcOptimal)
        //                             .setSrcQueueFamilyIndex(
        //                                 VK_QUEUE_FAMILY_IGNORED)
        //                             .setDstQueueFamilyIndex(
        //                                 VK_QUEUE_FAMILY_IGNORED)
        //                             .setSrcStageMask(
        //                                 vk::PipelineStageFlagBits2::eTransfer)
        //                             .setSrcAccessMask(
        //                                 vk::AccessFlagBits2::eTransferWrite)
        //                             .setDstStageMask(
        //                                 vk::PipelineStageFlagBits2::eTransfer)
        //                             .setDstAccessMask(
        //                                 vk::AccessFlagBits2::eTransferRead)
        //                             .setSubresourceRange(
        //                                 vk::ImageSubresourceRange()
        //                                     .setAspectMask(
        //                                         vk::ImageAspectFlagBits::eColor)
        //                                     .setLayerCount(
        //                                         layer_count)
        //                                     .setBaseArrayLayer(
        //                                         0)
        //                                     .setLevelCount(
        //                                         1)
        //                                     .setBaseMipLevel(
        //                                         i))));
        // #endif
        //                                    });
    }
    SetImageLayout(
        vk::ImageLayout::eGeneral,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader |
            vk::PipelineStageFlagBits::eComputeShader);
}

Image::~Image()
{
    auto& device = Get_Context_Singleton()
                       ->get_device()
                       ->get_handle();
    for (auto i : m_image_views)
        device.destroyImageView(i);
    if (memory) {

        device.freeMemory(memory);
        device.destroyImage(m_handle);
    }
}

} // namespace MoCheng3D