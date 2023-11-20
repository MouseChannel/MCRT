#include "Wrapper/Texture.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
// #if !defined(STB_IMAGE_IMPLEMENTATION)
// #define STB_IMAGE_IMPLEMENTATION
// #endif
#include "Tool/stb_image.h"

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#endif
namespace MCRT {
std::vector<std::shared_ptr<Texture>> Texture::textures;

Texture::Texture(std::string_view path)
{
    int width, height, channel;

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto m_app = Context::Get_Singleton()->Get_app();
    AAsset* file = AAssetManager_open(m_app->activity->assetManager,
                                      ((std::string)path).c_str(), AASSET_MODE_BUFFER);
    
    size_t fileLength = AAsset_getLength(file);

    std::vector<stbi_uc> source;
    source.resize(fileLength);

    AAsset_read(file, source.data(), fileLength);
    AAsset_close(file);

    stbi_uc* pixels = stbi_load_from_memory(source.data(), fileLength, &width, &height, &channel, STBI_rgb_alpha);
#else

    //    stbi_uc* pixels
    //        = stbi_load(file_name.data(), &w, &h, &channel, STBI_rgb_alpha);
    void* pixels = stbi_load(path.data(), &width, &height, &channel, STBI_rgb_alpha);
#endif
    size_t size = width * height * 4;
    if (!pixels) {
        throw std::runtime_error("image load failed");
    }
    image.reset(new Image(
        width,
        height,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1));
    image->SetImageLayout(vk::ImageLayout::eTransferDstOptimal,
                          vk::AccessFlagBits::eNone,
                          vk::AccessFlagBits::eTransferWrite,
                          vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::PipelineStageFlagBits::eTransfer);
    image->FillImageData(size, pixels);
    image->SetImageLayout(
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR);
}

Texture::Texture(void* data, uint32_t width, uint32_t height, uint32_t size)
{
    image.reset(new Image(
        width,
        height,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1));
    image->SetImageLayout(vk::ImageLayout::eTransferDstOptimal,
                          vk::AccessFlagBits::eNone,
                          vk::AccessFlagBits::eTransferWrite,
                          vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::PipelineStageFlagBits::eTransfer);
    image->FillImageData(size, data);
    image->SetImageLayout(
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR);
}

Texture::~Texture()
{
    image.reset();
}

std::vector<std::shared_ptr<Image>> Texture::get_image_handles()
{

    std::vector<std::shared_ptr<Image>> image_handles;

    image_handles.reserve(textures.size());
    for (auto& i : textures) {
        image_handles.emplace_back(i->image);
    }

    // add white textures
    std::shared_ptr<Image> white_image{
        new Image(1,
                  1,
                  vk::Format::eR8G8B8A8Srgb,
                  vk::ImageType::e2D,
                  vk::ImageTiling::eOptimal,
                  vk::ImageUsageFlagBits::eSampled,
                  vk::ImageAspectFlagBits::eColor,
                  vk::SampleCountFlagBits::e1)
    };
    white_image->SetImageLayout(
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eRayTracingShaderKHR);
    image_handles.push_back(white_image);
    return image_handles;
}
}
