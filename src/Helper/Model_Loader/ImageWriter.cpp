#include "Helper/Model_Loader/ImageWriter.hpp"
// #include "Rendering/Context.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"
#include <Helper/CommandManager.hpp>

#include "Tool/stb_image_write.h"

// #include <Wrapper/Skybox.hpp>
#include "Helper/Debugger.hpp"
#include <filesystem>

namespace MCRT {
void ImageWriter::WriteImage(std::shared_ptr<Image> image)
{
    // auto origin_format = image->Get_image_layout();
    // image->SetImageLayout(vk::ImageLayout::eGeneral,
    //                       vk::AccessFlagBits::eNone,
    //                       vk::AccessFlagBits::eNone,
    //                       vk::PipelineStageFlagBits::eTopOfPipe,
    //                       vk::PipelineStageFlagBits::eBottomOfPipe);
    int channel = 4;
    auto image_size = image->width * image->height * channel;
    // auto wfw = sizeof(float);
    auto dst_buffer = std::shared_ptr<Buffer>(
        new Buffer(
            image_size,
            vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
    Context::Get_Singleton()->get_debugger()->set_name(dst_buffer, "Image write buffer");

    CommandManager::ExecuteCmd(Context::Get_Singleton()
                                   ->get_device()
                                   ->Get_Graphic_queue(),
                               [&](auto cmd) {
                                   vk::BufferImageCopy region;
                                   vk::ImageSubresourceLayers subsource;
                                   subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                                       .setBaseArrayLayer(0)
                                       .setMipLevel(0)
                                       .setLayerCount(1);
                                   region.setBufferImageHeight(0)
                                       .setBufferOffset(0)
                                       .setImageOffset(0)
                                       .setImageExtent({ image->width, image->height, 1 })
                                       .setBufferRowLength(0)
                                       .setImageSubresource(subsource);
                                   cmd.copyImageToBuffer(image->get_handle(), image->Get_image_layout(), dst_buffer->get_handle(), region);
                               }

    );
    auto mapped_data = (dst_buffer->Get_mapped_data());

    std::vector<int> temp(mapped_data.size());

    for (int i = 0; i < mapped_data.size(); i++) {
        // set alpha = 1
        //  if (i % 4 == 3) {
        //      mapped_data[i] = 255;
        //  }
        temp[i] = mapped_data[i];
    }

    stbi_write_png("testout.png", image->width, image->height, channel, mapped_data.data(), image->width * channel);
}

//void ImageWriter::ReadImage(std::string path)
//{
//     
//}

void ImageWriter::WriteCubemap(std::shared_ptr<Image> cubemap)
{
    std::array<std::string, 6> names {
        "right",
        "left",
        "top",
        "bottom",
        "forward",
        "backward"
    };
    auto channel { 4 };
    auto single_image_size = cubemap->width * cubemap->height * channel;
    auto dst_buffer = std::shared_ptr<Buffer>(
        new Buffer(
            single_image_size * 6,
            vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
    CommandManager::ExecuteCmd(Context::Get_Singleton()
                                   ->get_device()
                                   ->Get_Graphic_queue(),
                               [&](auto cmd) {
                                   std::vector<vk::BufferImageCopy> regions(names.size());
                                   size_t offset = 0;
                                   for (int i = 0; i < 6; i++) {

                                       vk::ImageSubresourceLayers subsource;
                                       subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                                           .setBaseArrayLayer(i)
                                           .setMipLevel(0)
                                           .setLayerCount(1);
                                       regions[i].setBufferImageHeight(0).setBufferOffset(offset)
                                           // .setImageOffset(0)
                                           .setImageExtent({ cubemap->width, cubemap->height, 1 })
                                           .setBufferRowLength(0)
                                           .setImageSubresource(subsource);
                                       offset += single_image_size;
                                   }
                                   cmd.copyImageToBuffer(cubemap->get_handle(), cubemap->Get_image_layout(), dst_buffer->get_handle(), regions);
                               }

    );
    auto mapped_data = (dst_buffer->Get_mapped_data());

    // std::vector<int> temp(mapped_data.size());

    // for (int i = 0; i < mapped_data.size(); i++) {
    //     //set alpha = 1
    //     if (i % 4 == 3) {
    //         mapped_data[i] = 255;
    //     }
    // }

    std::filesystem::path out_dir { "assets/Cubemap/out" };

    if (!std::filesystem::exists(out_dir)) {
        std::filesystem::create_directory(out_dir);
    }
    for (int i = 0; i < 6; i++) {

        auto tt = out_dir / (names[i] + ".png");

        // std::vector<uint8_t> temp(mapped_data.begin()+)
        stbi_write_png(tt.string().c_str(),
                       cubemap->width,
                       cubemap->height,
                       channel,
                       mapped_data.data() + single_image_size * i,
                       cubemap->width * channel);
    }
}

}
