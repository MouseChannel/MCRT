#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {

void AccelerationStructure::build(std::shared_ptr<Buffer> scratch_buffer)
{

    dst_buffer = Buffer::CreateDeviceBuffer(nullptr,
                                            size_info.accelerationStructureSize,
                                            vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

    Context::Get_Singleton()->get_debugger()->set_name(dst_buffer, "dst_buffer");
    auto as_create_info = vk::AccelerationStructureCreateInfoKHR {}
                              .setType(m_type)
                              .setSize(size_info.accelerationStructureSize)
                              .setBuffer(dst_buffer->get_handle());
    // create   structure_handle here
    m_handle = Context::Get_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createAccelerationStructureKHR(as_create_info);
    // fill build_info part_2
    build_info.setDstAccelerationStructure(m_handle)
        .setScratchData(scratch_buffer->get_address());

    CommandManager::ExecuteCmd(Context::Get_Singleton()
                                   ->get_device()
                                   ->Get_Graphic_queue(),
                               [&](vk::CommandBuffer cmd_buffer) {
                                   // build  here
                                   cmd_buffer.setCheckpointNV(123);
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