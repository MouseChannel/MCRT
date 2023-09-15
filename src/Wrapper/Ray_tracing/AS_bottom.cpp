#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {
AccelerationStructure_Bottom::AccelerationStructure_Bottom(std::shared_ptr<Mesh> obj)
    : AccelerationStructure(vk::AccelerationStructureTypeKHR::eBottomLevel)
    , obj_index(obj->get_instance_index())
    , m_mesh(obj)
    , model_matrix(obj->get_transform())

{
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
void AccelerationStructure_Bottom::object_to_vkGeometryKHR(std::shared_ptr<Mesh> obj)
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
                         .setVertexStride(sizeof(Vertex))
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
        .setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastBuild | vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate)
        .setGeometries(geom_ready_data.as_geom);
    range_info = geom_ready_data.offset;
    auto prim_count { range_info.primitiveCount };
    // std::cout << "prim_count" << prim_count << std::endl;

    size_info = Context::Get_Singleton()
                    ->get_device()
                    ->get_handle()
                    .getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
                                                           build_info,
                                                           prim_count);
    // std::cout << "size_info" << size_info.accelerationStructureSize << std::endl;
}
void AccelerationStructure_Bottom::update(std::shared_ptr<Buffer> scratch_buffer)
{
    object_to_vkGeometryKHR(m_mesh);
    fill_build_info();
    build_info.setMode(vk::BuildAccelerationStructureModeKHR::eUpdate)
        .setSrcAccelerationStructure(m_handle)
        .setDstAccelerationStructure(m_handle)
        .setScratchData(scratch_buffer->get_address());
    CommandManager::ExecuteCmd(Context::Get_Singleton()
                                   ->get_device()
                                   ->Get_Graphic_queue(),
                               [&](vk::CommandBuffer cmd_buffer) {
                                   // build  here

                                   cmd_buffer.buildAccelerationStructuresKHR(build_info, &range_info);

                                   vk::MemoryBarrier2 barrier;
                                   barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR)
                                       .setSrcAccessMask(vk::AccessFlagBits2::eAccelerationStructureWriteKHR)
                                       .setDstStageMask(vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR)
                                       .setDstAccessMask(vk::AccessFlagBits2::eAccelerationStructureReadKHR);

                                   //    cmd_buffer.pipelineBarrier2(vk::DependencyInfo {}.setMemoryBarriers(barrier));
                               });
}
}
