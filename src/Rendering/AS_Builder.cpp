
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Device.hpp"
#include <vulkan/vulkan_core.h>

namespace MCRT {
AS_Builder::AS_Builder(std::shared_ptr<Device> device)
    : m_device(device)
{
}
Obj_geom_data AS_Builder::object_to_vkGeometryKHR(std::shared_ptr<Model> obj)
{

    auto vertexs_buffer = obj->get_vertex_buffer();
    auto indices_buffer = obj->get_indices_buffer();
    vk::AccelerationStructureGeometryTrianglesDataKHR triangles;
    vk::DeviceOrHostAddressConstKHR index_data;

    index_data.setDeviceAddress(indices_buffer->get_address());
    triangles.setIndexData(index_data)
        .setIndexType(vk::IndexType::eUint32)
        .setVertexData(vertexs_buffer->get_address())
        .setVertexStride(sizeof(VertexObj))
        .setVertexFormat(vk::Format::eR32G32B32Sfloat)
        .setMaxVertex(obj->get_vertex_count());
    vk::AccelerationStructureGeometryDataKHR geom_data;
    geom_data.setTriangles(triangles);
    // as_geom
    vk::AccelerationStructureGeometryKHR as_geom;
    as_geom.setGeometryType(vk::GeometryTypeKHR::eTriangles)
        .setGeometry(geom_data);
    // offset
    vk::AccelerationStructureBuildRangeInfoKHR offset;
    offset.setFirstVertex(0)
        .setPrimitiveCount(obj->get_vertex_count() / 3)
        .setPrimitiveOffset(0)
        .setTransformOffset(0);
    return Obj_geom_data {
        .as_geom { as_geom },
        .offset { offset }
    };
}
void AS_Builder::build_blas()
{
    auto count = blas_ready_data.size();
    build_as.resize(count);

    vk::DeviceSize total_as_build_size { 0 };
    vk::DeviceSize max_scratch_size { 0 };
    for (int i = 0; i < count; i++) {
        auto& build_info = build_as[i].build_info;

        auto& range_info = build_as[i].range_info;
        auto& size_info = build_as[i].size_info;
        auto& cur_obj_as_geom = blas_ready_data[i];

        build_info.setType(vk::AccelerationStructureTypeKHR::eBottomLevel)
            .setMode(vk::BuildAccelerationStructureModeKHR ::eBuild)
            .setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastBuild)
            .setGeometries(cur_obj_as_geom.as_geom);
        range_info = cur_obj_as_geom.offset;
        std::vector<uint32_t> max_prim_count(range_info.size());
        for (auto i = 0; i < max_prim_count.size(); i++) {
            max_prim_count[i] = range_info[i].primitiveCount;
        }

        size_info = m_device->Get_handle()
                        .getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
                                                               build_info,
                                                               max_prim_count);
        total_as_build_size += size_info.accelerationStructureSize;
        max_scratch_size = std::max(max_scratch_size, size_info.buildScratchSize);
    }
    scratch_buffer = Buffer::CreateDeviceBuffer(
        nullptr,
        max_scratch_size,
        vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer);
    Context::Get_Singleton()->get_debugger()->set_name(scratch_buffer, "scratch_buffer");
    // batch create as

    {
        std::vector<uint32_t> indices; // Indices of the BLAS to create
        vk::DeviceSize batchSize { 0 };
        vk::DeviceSize batchLimit { 256'000'000 }; // 256 MB
        for (auto i = 0; i < count; i++) {

            indices.push_back(i);
            batchSize += build_as[i].size_info.accelerationStructureSize;
            if (batchSize < batchLimit || i == count - 1) {
                // actually create blas
                for (auto idx : indices) {
                    auto& build_info = build_as[i].build_info;
                    auto& dst_as = build_as[i].as;

                    vk::AccelerationStructureCreateInfoKHR as_create_info;
                    build_as[i].dst_buffer = Buffer::CreateDeviceBuffer(nullptr, build_as[i].size_info.accelerationStructureSize, vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

                    Context::Get_Singleton()->get_debugger()->set_name(build_as[i].dst_buffer, "dst_buffer");
                    as_create_info.setType(vk::AccelerationStructureTypeKHR::eBottomLevel)
                        .setSize(build_as[i].size_info.accelerationStructureSize)
                        .setBuffer(build_as[i].dst_buffer->Get_handle());
                    // create blas structure here
                    dst_as = m_device->Get_handle()
                                 .createAccelerationStructureKHR(as_create_info);
                    build_info.setDstAccelerationStructure(dst_as)
                        .setScratchData(scratch_buffer->get_address());
                }

                auto graphic_queue = Context::Get_Singleton()
                                         ->get_device()
                                         ->Get_Graphic_queue();
                CommandManager::ExecuteCmd(graphic_queue, [&](auto cmd_buffer) {
                    // build blas here
                    cmd_buffer.buildAccelerationStructuresKHR(build_as[i].build_info, build_as[i].range_info.data());

                    vk::MemoryBarrier barrier;
                    barrier.setSrcAccessMask(vk::AccessFlagBits::eAccelerationStructureWriteKHR)
                        .setDstAccessMask(vk::AccessFlagBits::eAccelerationStructureReadKHR);
                    cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
                                               vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
                                               {},
                                               barrier,
                                               {},
                                               {});
                });
            }
        }
    }

    // a.setBuffer(vk::Buffer buffer_) auto as = m_device->Get_handle().createAccelerationStructureKHR(a);
}
vk::DeviceAddress AS_Builder::get_blas_device_address(uint32_t blas_index)
{
    vk::AccelerationStructureDeviceAddressInfoKHR address_info;
    address_info.setAccelerationStructure(build_as[blas_index].as);
    vk::DeviceAddress a;
    return a;
}
void AS_Builder::build_tlas()
{
    for (auto& obj_instance : Model::obj_instances) {
        auto& model = Model::models[obj_instance.obj_index];
        vk::AccelerationStructureInstanceKHR instance;
        // instance.setTransform(obj_instance.matrix).setInstanceCustomIndex(obj_instance.obj_index).setAccelerationStructureReference(uint64_t accelerationStructureReference_)
    }
}

}