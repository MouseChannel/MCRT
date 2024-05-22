#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <string_view>
#include <vector>
namespace MCRT {
class Image;
class Texture {
public:
    Texture(std::string_view path);
    // Texture(void* data, uint32_t width, uint32_t height, uint32_t size, bool linear = false, vk::Format format = vk::Format::eR8G8B8A8Srgb);
    // Texture(void* data, uint32_t width, uint32_t height, uint32_t size, bool linear = false, vk::Format format = vk::Format::eR8G8B8A8Srgb, bool mipmap = false);

    Texture(void* data, uint32_t width, uint32_t height, uint32_t size, vk::Format format, int mipmap_level = 1);

    ~Texture();
    [[nodiscard]] auto get_image()
    {
        return image;
    }
    static std::vector<std::shared_ptr<Texture>> textures;
    static std::vector<std::shared_ptr<Image>> get_image_handles();

private:
    std::shared_ptr<Image> image;
};

}