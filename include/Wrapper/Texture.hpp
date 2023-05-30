#pragma once
#include <string_view>
namespace MCRT {
class Image;
class Texture {
public:
    Texture(std::string_view path);
    Texture(void* data, uint32_t width, uint32_t height, uint32_t size);
    ~Texture();
    static std::vector<std::shared_ptr<Texture>> textures;
    static std::vector<std::shared_ptr<Image>> get_image_handles();

private:
    std::shared_ptr<Image> image;
};

}