#pragma once
#include "Rendering/Material.hpp"
#include "glm/glm.hpp"
#include "shader/Data_struct.h"
#include <string_view>
#include <vulkan/vulkan.hpp>
#include <memory>

namespace MCRT {
struct Triangle {

    float area;
    std::array<glm::vec3, 3> pos;
    Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        pos = { a, b, c };
        auto e1 = a - b;
        auto e2 = c - a;
        area = glm::length(glm::cross(e1, e2)) * 0.5f;
    }
};

struct ObjInstance {
    int obj_index;
    vk::TransformMatrixKHR matrix;
};
class Buffer;
class Texture;
class Mesh {
public:
    Mesh(std::string name,
         std::vector<Vertex>& vertexs,
         std::vector<uint32_t>& indexs,
         std::vector<Triangle>& triangles,
         Material material,
         std::array<std::array<float, 4>, 3> transform = std::array<std::array<float, 4>, 3> {
             { 1,
               0,
               0,
               0,
               0,
               1,
               0,
               0,
               0,
               0,
               1,
               0 } });
    [[nodiscard]] auto get_vertex_buffer()
    {
        return vertexs_buffer;
    }
    [[nodiscard]] auto get_indices_buffer()
    {
        return indices_buffer;
    }
    [[nodiscard]] auto get_material_buffer()
    {
        return material_buffer;
    }
    [[nodiscard]] auto get_vertex_count()
    {
        return m_index.size();
    }
    static std::vector<ObjInstance> obj_instances;
    static std::vector<std::shared_ptr<Mesh>> meshs;

    static void update_accelerate_structure_data();
    void set_index(int index)
    {
        obj_instance_index = index;
    }
    auto get_instance_index()
    {
        return obj_instance_index;
    }
    auto get_transform()
    {
        return tramsform;
    }
    auto get_pos()
    {
        return glm::vec3 {
            tramsform[0][3],
            tramsform[1][3],
            tramsform[2][3]
        };
    }
    auto get_model_matrix()
    {
        auto model_matrix = glm::mat4(1);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {

                model_matrix[i][j] = tramsform[i][j];
            }
        }
        return model_matrix;
    }

    Material m_material;

private:
    std::string m_name;
    std::vector<Vertex> m_vertexs;
    std::vector<uint32_t> m_index;
    std::shared_ptr<Buffer> vertexs_buffer;
    // std::shared_ptr<Buffer> position_buffer;
    // std::shared_ptr<Buffer> uv_buffer;

    std::shared_ptr<Buffer> indices_buffer;
    std::shared_ptr<Buffer> material_buffer;
    int obj_instance_index;

    std::array<std::array<float, 4>, 3>
        tramsform {
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0
        };
    std::vector<Triangle> triangles;
};

} // namespace MCRT