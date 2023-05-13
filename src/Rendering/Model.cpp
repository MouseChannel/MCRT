#include "Rendering/Model.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "Tool/tiny_obj_loader.hpp"
#include "Wrapper/Buffer.hpp"
namespace MCRT {
std::vector<ObjInstance> Model::obj_instances;
std::vector<std::shared_ptr<Model>> Model::models;
Model::Model(std::string name,
             std::vector<Vertex> vertexs,
             std::vector<uint32_t> indexs,
             Material material)
    : m_name(name)
    , m_vertexs(std::move(vertexs))
    , m_index(std::move(indexs))
    , m_material(material)
{
    assert(m_vertexs.size() > 0);
    auto ray_tracing_flag =
        vk::BufferUsageFlagBits::eShaderDeviceAddress |
        vk::BufferUsageFlagBits::eStorageBuffer |
        vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
    vertexs_buffer = Buffer::CreateDeviceBuffer(
        m_vertexs.data(),
        m_vertexs.size() * sizeof(m_vertexs[0]),
        vk::BufferUsageFlagBits::eVertexBuffer | ray_tracing_flag);
    indices_buffer = Buffer::CreateDeviceBuffer(
        m_index.data(),
        m_index.size() * sizeof(m_index[0]),
        vk::BufferUsageFlagBits::eIndexBuffer | ray_tracing_flag);
    material_buffer = Buffer::CreateDeviceBuffer(&m_material, sizeof(m_material), ray_tracing_flag);
}

void Model::update_accelerate_structure_data()
{
    for (int i = 0; i < models.size(); i++) {
        models[i]->set_index(i);
    }
    obj_instances.clear();
    for (auto i { 0 }; i < models.size(); i++) {
        obj_instances.emplace_back(ObjInstance {
            .obj_index = i,
            .matrix { vk::TransformMatrixKHR {}.setMatrix(models[i]->tramsform) } });
    }
}

} // namespace MCRT