#pragma once
#include <cstring>
#include <memory>
#include <string>
// #include <string_view>
#include <array>

#include <tuple>

namespace MCRT {
class Texture;
class Image;
class Mesh;

class Skybox {
public:
     std::array<std::string, 6> names{
        "right",
        "left",
        "top",
        "bottom",
        "forward",
        "backward"
    };
    Skybox(const Skybox&) = default;
    Skybox(Skybox&&) = delete;
    Skybox& operator=(const Skybox&) = default;
    Skybox& operator=(Skybox&&) = delete;
    Skybox(int height, int width);
    // Skybox(std::string file_dir);
    Skybox(std::string hdr_path, int cubemap_size = 1024);
    ~Skybox();

    [[nodiscard]] auto get_handle()
    {
        return cube_map;
    }

    [[nodiscard]] auto get_mesh()
    {
        return skybox_mesh;
    }

private:
    // std::vector<std::shared_ptr<Texture>> cube_maps;
    void Init(std::string face_dir);
    std::shared_ptr<Image> cube_map;

    std::shared_ptr<Mesh> skybox_mesh;
};

}
