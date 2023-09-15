#include "Wrapper/Skybox.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Texture.hpp"
#include "vulkan/vulkan.hpp"
#include <cmath>

namespace MCRT {
Skybox::Skybox(std::string file_dir)
{
    // skybox_mesh = GLTF_Loader::load_skybox("D:/MoCheng/MoChengRT/assets/cube.gltf");
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