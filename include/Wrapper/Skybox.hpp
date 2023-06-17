#pragma once
#include <string>
#include <string_view>

namespace MCRT {
class Texture;
class Image;
class Skybox {
public:
    enum {
        Top,
        Bottom,
        Left,
        Right,
        Forward,
        BackWard
    };
    std::array<std::string, 6> names {
        "right", "left", "top", "bottom", "forward", "backward"

        // "top", "forward", "left", "bottom",

        // "right",

        // "backward"
    };
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