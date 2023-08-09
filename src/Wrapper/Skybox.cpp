#include "Wrapper/Skybox.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Texture.hpp"
#include "vulkan/vulkan.hpp"
#include <cmath>

namespace MCRT {
Skybox::Skybox(std::string file_dir)
{
    names[0] = "right";
    names[1] = "left";
    names[2] = "top";
    names[3] = "bottom";
    names[4] = "forward";
    names[5] = "backward";

    std::vector<void*> datas(6);
    std::vector<size_t> sizes(6);
    int width = 0, height = 0, channel = 0;
    int p_width = 0, p_height = 0;

    for (int i = 0; i < 6; i++) {
        auto name = file_dir + "/" + names[i] + ".png";
        datas[i] = stbi_load(name.data(), &width, &height, &channel, STBI_rgb_alpha);

        { // check vaild
            std::cout << width << ' ' << height << std::endl;
            if (i > 0 && (width != p_width || height != p_height)) {
                throw std::runtime_error("failed to load cube_map");
            }
            p_width = width, p_height = height;
            if (!datas[i]) {
                throw std::runtime_error("failrd to load cube_map");
            }
        }
        sizes[i] = width * height * 4;
    }

    // vk::ImageCreateInfo image_create_info;

    // image_create_info.setImageType(vk::ImageType ::e2D)
    //     .setFormat(vk::Format ::eR8G8B8A8Unorm)
    //     .setSharingMode(vk::SharingMode ::eExclusive)
    //     .setArrayLayers(6)
    //     .setSamples(vk::SampleCountFlagBits::e1)
    //     .setTiling(vk::ImageTiling ::eOptimal)
    //     .setInitialLayout(vk::ImageLayout ::eUndefined)
    //     .setExtent(vk::Extent3D()
    //                    .setHeight(height)
    //                    .setWidth(width)
    //                    .setDepth(1))
    //     .setMipLevels(std::log2(std::min(width, height)))
    //     .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage)
    //     .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
    // auto image_handle = Context::Get_Singleton()
    //                         ->get_device()
    //                         ->get_handle()
    //                         .createImage(image_create_info);
    // cube_map.reset(new Image(image_handle,
    //                          image_create_info.format,
    //                          width,
    //                          height));
    cube_map.reset(new Image(width,
                             height,
                             vk::Format::eR8G8B8A8Unorm,
                             vk::ImageType::e2D,
                             vk::ImageTiling::eOptimal,
                             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage,
                             vk::ImageAspectFlagBits::eColor,
                             vk::SampleCountFlagBits::e1,
                             6,
                             std::log2(std::min(width, height)),
                             vk::ImageCreateFlagBits::eCubeCompatible));

    cube_map->SetImageLayout(vk::ImageLayout::eTransferDstOptimal,
                             vk::AccessFlagBits::eNone,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::PipelineStageFlagBits::eTopOfPipe,
                             vk::PipelineStageFlagBits::eTransfer);
    cube_map->FillImageData(sizes, datas);
    // cube_map->SetImageLayout(
    //     vk::ImageLayout::eGeneral,
    //     vk::AccessFlagBits::eTransferWrite,
    //     vk::AccessFlagBits::eShaderRead,
    //     vk::PipelineStageFlagBits::eTransfer,
    //     vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR,
    //     6);
    cube_map->SetImageLayout(vk::ImageLayout::eTransferSrcOptimal,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::AccessFlagBits::eTransferRead,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eTransfer);
    cube_map->generate_mipmap();
    cube_map->SetImageLayout(
        vk::ImageLayout::eGeneral,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR);
}
Skybox::Skybox(int height, int width)
{

    // vk::ImageCreateInfo image_create_info;

    // image_create_info.setImageType(vk::ImageType ::e2D)
    //     .setFormat(vk::Format ::eR8G8B8A8Snorm)
    //     .setSharingMode(vk::SharingMode ::eExclusive)
    //     .setArrayLayers(6)
    //     .setSamples(vk::SampleCountFlagBits::e1)
    //     .setTiling(vk::ImageTiling ::eOptimal)
    //     .setInitialLayout(vk::ImageLayout ::eUndefined)
    //     .setExtent(vk::Extent3D()
    //                    .setHeight(height)
    //                    .setWidth(width)
    //                    .setDepth(1))
    //     .setMipLevels(1)
    //     .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage)
    //     .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
    // auto image_handle = Context::Get_Singleton()
    //                         ->get_device()
    //                         ->get_handle()
    //                         .createImage(image_create_info);
    // cube_map.reset(new Image(image_handle,
    //                          image_create_info.format,
    //                          width,
    //                          height));
    cube_map.reset(new Image(width,
                             height,
                             vk::Format::eR8G8B8A8Unorm,
                             vk::ImageType::e2D,
                             vk::ImageTiling::eOptimal,
                             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage,
                             vk::ImageAspectFlagBits::eColor,
                             vk::SampleCountFlagBits::e1,
                             6,
                             1,
                             vk::ImageCreateFlagBits::eCubeCompatible));

    cube_map->SetImageLayout(
        vk::ImageLayout::eGeneral,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR);
}
Skybox::~Skybox()
{
}

}