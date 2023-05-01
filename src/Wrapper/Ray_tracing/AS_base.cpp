#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {

void AccelerationStructure::build(std::shared_ptr<Buffer> scratch_buffer)
{

    vk::AccelerationStructureCreateInfoKHR as_create_info;
    dst_buffer = Buffer::CreateDeviceBuffer(nullptr,
                                            size_info.accelerationStructureSize,
                                            vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

    Context::Get_Singleton()->get_debugger()->set_name(dst_buffer, "dst_buffer");
    as_create_info.setType(m_type)
        .setSize(size_info.accelerationStructureSize)
        .setBuffer(dst_buffer->Get_handle());
    // create   structure_handle here
    m_handle = Context::Get_Singleton()
                   ->get_device()
                   ->Get_handle()
                   .createAccelerationStructureKHR(as_create_info);
    // fill build_info part_2
    build_info.setDstAccelerationStructure(m_handle)
        .setScratchData(scratch_buffer->get_address());

    CommandManager::ExecuteCmd(Context::Get_Singleton()
                                   ->get_device()
                                   ->Get_Graphic_queue(),
                               [&](vk::CommandBuffer cmd_buffer) {
                                   // build  here
                                   cmd_buffer.buildAccelerationStructuresKHR(build_info, &range_info);

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