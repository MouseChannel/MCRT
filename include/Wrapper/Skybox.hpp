#pragma once
#include <cstring>
#include <memory>
// #include <string>
// #include <string_view>
#include  <array>

#include <tuple>

namespace MCRT {
class Texture;
class Image;
class Skybox {
public:
     
    std::array<std::string, 6> names ;
    Skybox(const Skybox&) = default;
    Skybox(Skybox&&) = delete;
    Skybox& operator=(const Skybox&) = default;
    Skybox& operator=(Skybox&&) = delete;
    Skybox(int height, int width);
    Skybox(std::string file_dir);
    ~Skybox();

    [[nodiscard]] auto get_handle()
    {
        return cube_map;
    }

private:
    // std::vector<std::shared_ptr<Texture>> cube_maps;
    std::shared_ptr<Image> cube_map;
};

}