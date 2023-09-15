#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include "Helper/CommandManager.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Ray_Tracing/AS_bottom.hpp"

namespace MCRT {
AccelerationStructure_Top::AccelerationStructure_Top(std::vector<std::shared_ptr<AccelerationStructure_Bottom>> as_bottoms)
    : AccelerationStructure(vk::AccelerationStructureTypeKHR::eTopLevel)
    , bottoms_as(as_bottoms)
{
    instances.clear();
    for (auto i : as_bottoms) {
        add_as_bottom(i);
    }
    fill_build_info();
}
AccelerationStructure_Top::~AccelerationStructure_Top()
{
}

void AccelerationStructure_Top::add_as_bottom(std::shared_ptr<AccelerationStructure_Bottom> as_bottom)
{
    vk::AccelerationStructureInstanceKHR instance;
    // std::cout << as_bottom->get_model_matrix().matrix[0][0] << std::endl;
    instance.setTransform(as_bottom->get_model_matrix())
        .setInstanceCustomIndex(as_bottom->get_obj_index())
        .setAccelerationStructureReference(as_bottom->get_address())
        .setFlags(vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable)
        .setMask(0xff)
        .setInstanceShaderBindingTableRecordOffset(0);
    instances.emplace_back(instance);
}
void AccelerationStructure_Top::fill_build_info()
{
    instance_buffer = Buffer::create_buffer(instances,
                                            vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR);

    ready_data.address.setDeviceAddress(instance_buffer->get_address());
    // vk::AccelerationStructureGeometryInstancesDataKHR instance_geom;
    ready_data.instance_geom.setData(ready_data.address);

    ready_data.top_as_geom_data.setInstances(ready_data.instance_geom);

    ready_data.top_as_geom.setGeometryType(vk::GeometryTypeKHR::eInstances)
        .setGeometry(ready_data.top_as_geom_data);

    build_info.setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace | vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate)
        // .setGeometries(ready_data.top_as_geom)
        .setGeometries(ready_data.top_as_geom)

        .setMode(vk::BuildAccelerationStructureModeKHR ::eBuild)
        .setType(vk::AccelerationStructureTypeKHR ::eTopLevel);
    range_info.setPrimitiveCount(instances.size());
    size_info = Context::Get_Singleton()
                    ->get_device()
                    ->get_handle()
                    .getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR ::eDevice,
                                                           build_info,
                                                           instances.size());
}
void AccelerationStructure_Top::update(std::shared_ptr<Buffer> scratch_buffer)
{

    instances.clear();
    for (auto i : bottoms_as) {
        add_as_bottom(i);
    }
    fill_build_info();
    build_info.setMode(vk::BuildAccelerationStructureModeKHR ::eUpdate)
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
                               });
}
}
