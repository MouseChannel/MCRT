#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"
namespace MCRT {
CommandBuffer::CommandBuffer()
{

    vk::CommandBufferAllocateInfo allocate_info;

    auto command_pool = Get_Context_Singleton()->get_commandpool();
    allocate_info.setCommandBufferCount(1)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(command_pool->get_handle());

    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .allocateCommandBuffers(allocate_info)[0];
}
CommandBuffer::~CommandBuffer()
{
}
void CommandBuffer::Begin(vk::CommandBufferUsageFlags begin_flags)
{
    vk::CommandBufferBeginInfo begin_info;
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_handle.begin(begin_info);
}
void CommandBuffer::End()
{
    m_handle.end();
}

} // namespace MCRT