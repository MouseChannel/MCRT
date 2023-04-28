#pragma once
#include "glm/glm.hpp"
#include <string_view>
#include <vulkan/vulkan.hpp>

namespace MCRT {
struct VertexObj {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    //   void dd() { pos.x = 1.1f; }
};
struct ObjInstance {
    int obj_index;
    vk::TransformMatrixKHR matrix;
};
class Buffer;
class Model {
public:
    Model(std::string obj_path, std::string_view texture_path);
    [[nodiscard]] auto get_vertex_buffer()
    {
        return vertexs_buffer;
    }
    [[nodiscard]] auto get_indices_buffer()
    {
        return indices_buffer;
    }
    [[nodiscard]] auto get_vertex_count()
    {
        return vertexs.size();
    }
   constexpr static std::vector<ObjInstance> obj_instances;
   constexpr static std::vector<std::shared_ptr<Model>> models;

private:
    std::vector<VertexObj> vertexs;
    std::shared_ptr<Buffer> vertexs_buffer;
    std::shared_ptr<Buffer> indices_buffer;
};

} // namespace MCRT