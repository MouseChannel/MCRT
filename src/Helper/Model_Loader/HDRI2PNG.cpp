#include <filesystem>
// #include "Wrapper/Texture.hpp"
#include "Helper/Model_Loader/HDRI2PNG.hpp"
// #if !defined(STB_IMAGE_IMPLEMENTATION)
// #define STB_IMAGE_IMPLEMENTATION
// #endif
// #if !defined(STB_IMAGE_WRITE_IMPLEMENTATION)
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #endif
// #include "stb_image.h"
#include "Tool/stb_image_write.h"
// #include

namespace MCRT {

std::vector<stbi_uc>
HDRI_Helper::copyPixelBilinear(std::vector<stbi_uc> source_data, int source_width, int source_height, FACE face)
{
    //    const auto PI = std::numbers::pi_v<float>;
    const auto PI = 3.1415926f;

    auto channels = 3;
    auto readIndex = [&](int x, int y, int width) {
        return channels * (y * width + x);
    };
    auto max_width = 1024;
    std::vector<stbi_uc> dst_data(max_width * max_width * channels);
    for (int x = 0; x < max_width; x++) {
        for (int y = 0; y < max_width; y++) {

            // get position on cube face
            // cube is centered at the origin with a side length of 2
            auto cube = get_orientations(face,
                                         (2. * (x + .5) / max_width - 1),
                                         (2. * (y + .5) / max_width - 1));
            // project cube face onto unit sphere by converting cartesian to spherical coordinates
            auto r = glm::sqrt(cube.x * cube.x + cube.y * cube.y + cube.z * cube.z);
            auto lon = glm::mod(glm::atan(cube.y, cube.x) + 3.1415f, 2 * PI);
            auto lat = glm::acos(cube.z / r);

            // start copy
            auto xFrom = source_width * lon / PI / 2 - .5;
            auto yFrom = source_height * lat / PI - .5;

            auto dst_index = channels * (y * max_width + x);
            // set alpha
            //  dst_data[dst_index + 3] = 255;
            auto xl = glm::clamp(glm::floor(xFrom), 0., double(source_width - 1));
            auto xr = glm::clamp(glm::ceil(xFrom), 0., double(source_width - 1));
            auto xf = xFrom - xl;

            auto yl = glm::clamp(glm::floor(yFrom), 0., double(source_height - 1));
            auto yr = glm::clamp(glm::ceil(yFrom), 0., double(source_height - 1));
            auto yf = yFrom - yl;
            auto p00 = readIndex(xl, yl, source_width);
            auto p10 = readIndex(xr, yl, source_width);
            auto p01 = readIndex(xl, yr, source_width);
            auto p11 = readIndex(xr, yr, source_width);
            for (auto channel = 0; channel < 3; channel++) {
                auto p0 = source_data[p00 + channel] * (1 - xf) + source_data[p10 + channel] * xf;
                auto p1 = source_data[p01 + channel] * (1 - xf) + source_data[p11 + channel] * xf;
                dst_data[dst_index + channel] = glm::ceil(p0 * (1 - yf) + p1 * yf);
            }
        }
    }
    return dst_data;
}

void HDRI_Helper::HDR2Cubemap(std::string path, int cubemap_size)
{

    int width, height, channel;
    auto c = stbi_load(path.c_str(), &width, &height, &channel, STBI_rgb);
    std::vector<stbi_uc> data(c, c + (width * height * channel));
    // stbi_write_png("origin.png", width, height, channel, data.data(), width * channel);
    namespace fs = std::filesystem;

    auto dir_path = get_hdr_out_dir(path);
    if (!fs::exists(fs::path(dir_path))) {
        fs::create_directory(dir_path);
    }
    for (auto face = 0; face < FACE::face_count; face++) {

        auto temp = copyPixelBilinear(data, width, height, static_cast<FACE>(face));
        auto out_file_name = dir_path / (get_face_name((FACE)face) + ".png");
        stbi_write_png(
            out_file_name.string().c_str(),
            cubemap_size,
            cubemap_size,
            STBI_rgb,
            temp.data(),
            cubemap_size * STBI_rgb);
    }
}

};
