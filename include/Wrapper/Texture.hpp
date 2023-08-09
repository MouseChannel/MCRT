#pragma once
#include <string_view>
#include <memory>
#include <vector>
namespace MCRT {
class Image;
class Texture {
public:
    Texture(std::string_view path);
    Texture(void* data, uint32_t width, uint32_t height, uint32_t size);
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