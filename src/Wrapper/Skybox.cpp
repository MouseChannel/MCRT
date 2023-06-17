#include "Wrapper/Skybox.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Texture.hpp"
#include "vulkan/vulkan.hpp"

namespace MCRT {
Skybox::Skybox(std::string file_dir)
{
    // cube_maps.resize(6);
    // for (int i = 0; i < 6; i++) {
    //     cube_maps[i].reset(new Texture(file_dir + "/" + names[i] + ".png"));
    // }

    std::vector<void*> datas(6);
    std::vector<size_t> sizes(6);
    int width = 0, height = 0, channel = 0;
    int p_width = 0, p_height = 0;

    for (int i = 0; i < 6; i++) {
        auto name = file_dir + "/" + names[i] + ".png";
        datas[i] = stbi_load(name.data(), &width, &height, &channel, STBI_rgb_alpha);
        { // check vaild
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

    vk::ImageCreateInfo image_create_info;

    image_create_info.setImageType(vk::ImageType ::e2D)
        .setFormat(vk::Format ::eR8G8B8A8Srgb)
        .setSharingMode(vk::SharingMode ::eExclusive)
        .setArrayLayers(6)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling ::eOptimal)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setExtent(vk::Extent3D()
                       .setHeight(height)
                       .setWidth(width)
                       .setDepth(1))
        .setMipLevels(1)
        .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
        .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
    auto image_handle = Context::Get_Singleton()->get_device()->get_handle().createImage(image_create_info);
    cube_map.reset(new Image(image_handle,
                             image_create_info.format,
                             width,
                             height));

    cube_map->SetImageLayout(vk::ImageLayout::eTransferDstOptimal,
                             vk::AccessFlagBits::eNone,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::PipelineStageFlagBits::eTopOfPipe,
                             vk::PipelineStageFlagBits::eTransfer,
                             6);
    cube_map->FillImageData(sizes, datas);
    cube_map->SetImageLayout(
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR,
        6);
}
Skybox::~Skybox()
{
}

}