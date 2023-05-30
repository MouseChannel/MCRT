#include "Wrapper/Texture.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include "Tool/stb_image.h"

namespace MCRT {
std::vector<std::shared_ptr<Texture>> Texture::textures;
Texture::Texture(std::string_view path)
{
    int width, height, channel;
    stbi_uc* pixels = stbi_load(path.data(), &width, &height, &channel, STBI_rgb_alpha);
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
    std::shared_ptr<Image> white_image {
        new Image(1,
                  1,
                  vk::Format::eR32G32B32A32Sfloat,
                  vk::ImageType::e2D,
                  vk::ImageTiling::eOptimal,
                  vk::ImageUsageFlagBits::eStorage,
                  vk::ImageAspectFlagBits::eColor,
                  vk::SampleCountFlagBits::e1)
    };

    image_handles.push_back(white_image);
    return image_handles;
}
}