#include "Wrapper/Skybox.hpp"
#include "Helper/Model_Loader/HDRI2PNG.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Texture.hpp"
#include "vulkan/vulkan.hpp"
#include <cmath>

#include <filesystem>

namespace MCRT {

Skybox::Skybox(std::string hdr_path, int cubemap_size)
{
    // Init(hdr_path);
    // return;

    namespace fs = std::filesystem;
    auto get_filename = [&]() {
        auto file_name = fs::path(hdr_path).filename();
        if (hdr_path.find_last_of(".") != std::string::npos)
            return hdr_path.substr(0, hdr_path.find_last_of("."));
        return std::string();
    };
    auto dir_path = HDRI_Helper::get_hdr_out_dir(hdr_path);

    // auto dir_path = fs::path(path).parent_path() / "Cubemap" / file_name;
    if (!fs::exists(fs::path(dir_path))) {
        HDRI_Helper::HDR2Cubemap(hdr_path);
    }
    Init(dir_path.string());
}

void Skybox::Init(std::string face_dir)
{

    std::vector<void*> datas(6);
    std::vector<size_t> sizes(6);
    int width = 0, height = 0, channel = 0;
    int p_width = 0, p_height = 0;

    for (int i = 0; i < 6; i++) {
        auto name = face_dir + "/" + names[i] + ".png";
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        auto m_app = Context::Get_Singleton()->Get_app();
        AAsset* file = AAssetManager_open(m_app->activity->assetManager,
                                          ((std::string)name.data()).c_str(),
                                          AASSET_MODE_BUFFER);

        size_t fileLength = AAsset_getLength(file);

        std::vector<stbi_uc> source;
        source.resize(fileLength);

        AAsset_read(file, source.data(), fileLength);
        AAsset_close(file);

        datas[i] = stbi_load_from_memory(source.data(), fileLength, &width, &height, &channel, STBI_rgb_alpha);
#else
        datas[i] = stbi_load(name.data(), &width, &height, &channel, STBI_rgb_alpha);
#endif

        {
            // check vaild
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

// Skybox::Skybox(std::string file_dir)
// {
//     // skybox_mesh = GLTF_Loader::load_skybox("/home/mocheng/project/MCRT/assets/cube.gltf");
//
// }

Skybox::Skybox(int height, int width)
{

    cube_map.reset(new Image(width,
                             height,
                             vk::Format::eR8G8B8A8Unorm,
                             vk::ImageType::e2D,
                             vk::ImageTiling::eOptimal,
                             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc,
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
