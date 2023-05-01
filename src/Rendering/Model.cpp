#include "Rendering/Model.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "Tool/tiny_obj_loader.hpp"
#include "Wrapper/Buffer.hpp"
namespace MCRT {
std::vector<ObjInstance> Model::obj_instances;
std::vector<std::shared_ptr<Model>> Model::models;
Model::Model(std::string obj_path, std::string_view texture_path)
{
    tinyobj::ObjReader reader;
    reader.ParseFromFile(obj_path);

    auto attrib = reader.GetAttrib();
    auto shapes = reader.GetShapes();
    //   std::vector<float> m_positions;
    //   std::vector<float> m_normals;
    //   std::vector<float> m_uv;
    std::vector<uint32_t> m_index;
    for (const auto& shape : shapes)
        for (const auto& index : shape.mesh.indices) {
            vertexs.emplace_back(VertexObj {
                .pos { attrib.vertices[3 * index.vertex_index + 0],
                       attrib.vertices[3 * index.vertex_index + 1],
                       attrib.vertices[3 * index.vertex_index + 2] },
                .normal { attrib.normals[3 * index.normal_index + 0],
                          attrib.normals[3 * index.normal_index + 1],
                          attrib.normals[3 * index.normal_index + 2] },
                .texCoord { attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1] } });
            //   m_positions.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            //   m_positions.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            //   m_positions.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            //   m_normals.push_back(attrib.normals[3 * index.normal_index + 0]);
            //   m_normals.push_back(attrib.normals[3 * index.normal_index + 1]);
            //   m_normals.push_back(attrib.normals[3 * index.normal_index + 2]);

            //   m_uv.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
            //   m_uv.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index +
            //   1]);

            m_index.push_back(m_index.size());
        }
    assert(vertexs.size() > 0);
    auto r = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    auto ray_tracing_flag =
        vk::BufferUsageFlagBits::eShaderDeviceAddress |
        vk::BufferUsageFlagBits::eStorageBuffer |
        vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
    vertexs_buffer = Buffer::CreateDeviceBuffer(
        vertexs.data(),
        vertexs.size() * sizeof(vertexs[0]),
        vk::BufferUsageFlagBits::eVertexBuffer | ray_tracing_flag);
    indices_buffer = Buffer::CreateDeviceBuffer(
        m_index.data(),
        m_index.size() * sizeof(m_index[0]),
        vk::BufferUsageFlagBits::eIndexBuffer | ray_tracing_flag);
}
} // namespace MCRT