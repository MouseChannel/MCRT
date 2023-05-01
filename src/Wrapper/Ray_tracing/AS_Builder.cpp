
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include <vulkan/vulkan_core.h>

namespace MCRT {
AS_Builder::AS_Builder()

{
}

void AS_Builder::add_blas_obj(std::shared_ptr<Model> obj)
{

    m_accelerate_structures.emplace_back(new AccelerationStructure_Bottom(obj));
    Model::obj_instances.emplace_back(ObjInstance { .obj_index = (int)Model::obj_instances.size() });
}
 
void AS_Builder::build_blas()
{

    vk::DeviceSize max_scratch_sice { 0 };
    for (auto& i : m_accelerate_structures) {
        max_scratch_sice = std::max(max_scratch_sice, i->get_scratch_size());
    }
    assert(max_scratch_sice > 0);
    auto scratch_buffer = Buffer::create_buffer(nullptr, max_scratch_sice, vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer);

    auto count = m_accelerate_structures.size();
    std::vector<uint32_t> indices; // Indices of the BLAS to create
    vk::DeviceSize batchSize { 0 };
    vk::DeviceSize batchLimit { 256'000'000 }; // 256 MB
    for (auto i = 0; i < count; i++) {

        indices.push_back(i);
        batchSize += m_accelerate_structures[i]->get_structure_size();
        if (batchSize > batchLimit || i == count - 1) {
            // actually create blas
            for (auto idx : indices) {
                m_accelerate_structures[idx]->build(scratch_buffer);
            }

            indices.clear();
            batchSize = 0;
        }
    }

    // a.setBuffer(vk::Buffer buffer_) auto as = m_device->Get_handle().createAccelerationStructureKHR(a);
}

void AS_Builder::build_tlas()
{
    top_as.reset(new AccelerationStructure_Top(m_accelerate_structures));

    auto scratch_buffer = Buffer::create_buffer(nullptr,
                                                top_as->get_scratch_size(),
                                                vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
    top_as->build(scratch_buffer);
} 

}