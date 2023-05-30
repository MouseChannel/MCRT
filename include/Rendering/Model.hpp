#pragma once
#include "Rendering/Material.hpp"
#include "glm/glm.hpp"
#include "shader/Data_struct.h"
#include <string_view>
#include <vulkan/vulkan.hpp>

namespace MCRT {
// struct VertexObj {
//     glm::vec3 pos;
//     glm::vec3 normal;
//     glm::vec2 texCoord;

//     //   void dd() { pos.x = 1.1f; }
// };
struct ObjInstance {
    int obj_index;
    vk::TransformMatrixKHR matrix;
};
class Buffer;
class Texture;
class Mesh {
public:
    Mesh(std::string name,
         std::vector<Vertex> vertexs,
         std::vector<uint32_t> indexs,
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

private:
    std::string m_name;
    Material m_material;
    std::vector<Vertex> m_vertexs;
    std::vector<uint32_t> m_index;
    std::shared_ptr<Buffer> vertexs_buffer;
    // std::shared_ptr<Buffer> position_buffer;
    // std::shared_ptr<Buffer> uv_buffer;

    std::shared_ptr<Buffer> indices_buffer;
    std::shared_ptr<Buffer> material_buffer;
    int obj_instance_index;
    std::array<std::array<float, 4>, 3> tramsform {
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
};

} // namespace MCRT