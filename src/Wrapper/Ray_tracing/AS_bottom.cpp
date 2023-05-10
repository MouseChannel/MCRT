#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {
AccelerationStructure_Bottom::AccelerationStructure_Bottom(std::shared_ptr<Model> obj)
    : AccelerationStructure(vk::AccelerationStructureTypeKHR::eBottomLevel)
{
    obj_index = 0;
    model_matrix.setMatrix(std::array<std::array<float, 4>, 3> {
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
        0 });
    object_to_vkGeometryKHR(obj);
    fill_build_info();
}
AccelerationStructure_Bottom::~AccelerationStructure_Bottom()
{
}
vk::DeviceAddress AccelerationStructure_Bottom::get_address()
{
    vk::AccelerationStructureDeviceAddressInfoKHR address_info;
    address_info.setAccelerationStructure(m_handle);
    return Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .getAccelerationStructureAddressKHR(address_info);
}
void AccelerationStructure_Bottom::object_to_vkGeometryKHR(std::shared_ptr<Model> obj)
{
    // add tlas_instance
    // Model::obj_instances.push_back(ObjInstance { .obj_index = (int)blas_ready_data.size() });

    auto vertexs_buffer = obj->get_vertex_buffer();
    auto indices_buffer = obj->get_indices_buffer();

    // vk::DeviceOrHostAddressConstKHR index_data;

    // index_data.setDeviceAddress(indices_buffer->get_address());
    auto triangles = vk::AccelerationStructureGeometryTrianglesDataKHR()
                         .setIndexData(vk::DeviceOrHostAddressConstKHR {}
                                           .setDeviceAddress(indices_buffer->get_address()))
                         .setIndexType(vk::IndexType::eUint32)
                         .setVertexData(vertexs_buffer->get_address())
                         .setVertexStride(sizeof(VertexObj))
                         .setVertexFormat(vk::Format::eR32G32B32Sfloat)
                         .setMaxVertex(obj->get_vertex_count());
    // vk::AccelerationStructureGeometryDataKHR geom_data;
    // geom_data.setTriangles(triangles);
    // as_geom
    vk::AccelerationStructureGeometryKHR as_geom;
    as_geom.setGeometryType(vk::GeometryTypeKHR::eTriangles)
        .setGeometry(vk::AccelerationStructureGeometryDataKHR {}
                         .setTriangles(triangles));
    // offset
    vk::AccelerationStructureBuildRangeInfoKHR offset;
    offset.setFirstVertex(0)
        .setPrimitiveCount(obj->get_vertex_count() / 3)
        .setPrimitiveOffset(0)
        .setTransformOffset(0);

    geom_ready_data = Obj_geom_data {
        .as_geom { as_geom },
        .offset { offset }
    };
}

void AccelerationStructure_Bottom::fill_build_info()
{
    vk::DeviceSize scratch_size { 0 };
    build_info.setType(vk::AccelerationStructureTypeKHR::eBottomLevel)
        .setMode(vk::BuildAccelerationStructureModeKHR ::eBuild)
        .setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastBuild)
        .setGeometries(geom_ready_data.as_geom);
    range_info = geom_ready_data.offset;
    auto prim_count { range_info.primitiveCount };

    size_info = Context::Get_Singleton()
                    ->get_device()
                    ->get_handle()
                    .getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
                                                           build_info,
                                                           prim_count);
}

// void AccelerationStructure_Bottom::build(std::shared_ptr<Buffer> scratch_buffer)
// {

//     vk::AccelerationStructureCreateInfoKHR as_create_info;
//     dst_buffer = Buffer::CreateDeviceBuffer(nullptr, size_info.accelerationStructureSize, vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

//     Context::Get_Singleton()->get_debugger()->set_name(dst_buffer, "dst_buffer");
//     as_create_info.setType(vk::AccelerationStructureTypeKHR::eBottomLevel)
//         .setSize(size_info.accelerationStructureSize)
//         .setBuffer(dst_buffer->Get_handle());
//     // create blas structure here
//     m_handle = Context::Get_Singleton()
//                    ->get_device()
//                    ->Get_handle()
//                    .createAccelerationStructureKHR(as_create_info);
//     // fill build_info part_2
//     build_info.setDstAccelerationStructure(m_handle)
//         .setScratchData(scratch_buffer->get_address());

//     CommandManager::ExecuteCmd(Context::Get_Singleton()
//                                    ->get_device()
//                                    ->Get_Graphic_queue(),
//                                [&](vk::CommandBuffer cmd_buffer) {
//                                    // build blas here
//                                    cmd_buffer.buildAccelerationStructuresKHR(build_info, &range_info);

//                                    vk::MemoryBarrier barrier;
//                                    barrier.setSrcAccessMask(vk::AccessFlagBits::eAccelerationStructureWriteKHR)
//                                        .setDstAccessMask(vk::AccessFlagBits::eAccelerationStructureReadKHR);
//                                    cmd_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
//                                                               vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
//                                                               {},
//                                                               barrier,
//                                                               {},
//                                                               {});
//                                });
}
