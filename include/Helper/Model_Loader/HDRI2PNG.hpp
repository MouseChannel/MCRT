#pragma once



#include <Tool/stb_image.h>
#include "glm/glm.hpp"
#include "Wrapper/Image.hpp"
#include <filesystem>

namespace MCRT {
// std::unique_ptr<Context> Context::_instance { nullptr };

//


class HDRI_Helper {
public:
    enum FACE {
        pz,
        nz,
        px,
        nx,
        py,
        ny,
        face_count
    };

    static std::string get_face_name(FACE face)
    {
        switch (face) {
        case pz:
            return "forward";

        case nz:
            return "backward";

        case px:
            return "right";

        case nx:
            return "left";
        case py:
            return "top";

        case ny:
            return "bottom";

        }

    }

    static auto get_orientations(FACE face, float x, float y)
    {
        switch (face) {
        case pz:
            return glm::vec3(-1, -x, -y);
        case nz:
            return glm::vec3(1, x, -y);
        case px:
            return glm::vec3(x, -1, -y);
        case nx:
            return glm::vec3(-x, 1, -y);
        case py:
            return glm::vec3(-y, -x, 1);
        case ny:
            return glm::vec3(y, -x, -1);
        default:
            throw std::runtime_error("fail to make cubemap");
        }

    }

    static auto get_hdr_out_dir(std::string hdr_path)
    {
        namespace fs = std::filesystem;
        auto get_filename = [&](std::string hdr_path) {
            auto file_name = fs::path(hdr_path).filename().string();
            if (file_name.find_last_of(".") != std::string::npos)
                return file_name.substr(0, file_name.find_last_of("."));
            return std::string();
        };

        auto file_name = get_filename(hdr_path);
        auto dir_path = fs::path(hdr_path).parent_path()  / file_name;
        return dir_path;
        if (!fs::exists(fs::path(dir_path))) {
            fs::create_directory(dir_path);
        }

    }

    static std::vector<stbi_uc> copyPixelBilinear(std::vector<stbi_uc> source_data, int source_width, int source_height, FACE face);


    ///create 6 face png
    static void HDR2Cubemap(std::string path, int cubemap_size = 1024);


};


//
//
// void RenderFace(int max_width)
// {
//     auto face_texture = std::make_shared<MCRT::Image>(max_width,
//                                                       max_width,
//                                                       vk::Format::eR32G32B32Sfloat,
//                                                       vk::ImageType::e2D,
//                                                       vk::ImageTiling::eLinear,
//                                                       vk::ImageUsageFlagBits::eSampled,
//                                                       vk::ImageAspectFlagBits::eColor,
//                                                       vk::SampleCountFlagBits::e1);
//     for (int x = 0; x < max_width; x++) {
//         for (int y = 0; y < max_width; y++) {
//
//         }
//     }
//
// }


// std::vector<stbi_uc> copyPixelLanczos(std::vector<stbi_uc> source_data, int source_width, int source_height, FACE face)
// {
//     // auto kernel = x
//
// }


// void test()
// {
//     load_hdr("/home/mocheng/project/MCRT/assets/kart_club_2k.hdr");
//     return;
//
//     int width, height, channel;
//     auto data = stbi_load("/home/mocheng/project/MCRT/assets/kart_club_2k.hdr", &width, &height, &channel, 0);
//     std::vector<stbi_uc> cdata(data, data + (width * height * channel));
//
//     std::vector<stbi_uc> data_vector(200 * 200 * 4, 255);
//     for (int i = 0; i < data_vector.size(); i += 4) {
//         data_vector[i] = 0;
//         data_vector[i + 1] = 0;
//
//     }
//
//     stbi_write_png("mochengdelete.png",
//                    width,
//                    height,
//                    channel,
//                    cdata.data(),
//                    width * 3
//         );
// }

}
