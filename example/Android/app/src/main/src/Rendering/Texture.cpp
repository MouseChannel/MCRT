// #define STB_IMAGE_IMPLEMENTATION
#include "Rendering/Texture.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Image.hpp"

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#endif

namespace MoCheng3D {

Texture::Texture(std::string_view file_name)
{
    int w, h, channel;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto m_app = Context::Get_Singleton()->Get_app();
    AAsset* file = AAssetManager_open(m_app->activity->assetManager,
                                      ((std::string)file_name).c_str(), AASSET_MODE_BUFFER);

    size_t fileLength = AAsset_getLength(file);

    std::vector<stbi_uc> source;
    source.resize(fileLength);

    AAsset_read(file, source.data(), fileLength);
    AAsset_close(file);

    stbi_uc* pixels = stbi_load_from_memory(source.data(), fileLength, &w, &h, &channel, STBI_rgb_alpha);
#else

    stbi_uc* pixels
        = stbi_load(file_name.data(), &w, &h, &channel, STBI_rgb_alpha);
#endif
    size_t size = w * h * 4;
    if (!pixels) {
        throw std::runtime_error("image load failed");
    }
    image.reset(new Image(
        w, h, vk::Format::eR8G8B8A8Srgb, vk::ImageType::e2D,
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
        vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader);
    stbi_image_free(pixels);
}
Texture::~Texture() { image.reset(); }

}
