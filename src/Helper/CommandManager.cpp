#include "Helper/CommandManager.hpp"
#include "Wrapper/CommandBuffer.hpp"

namespace MCRT {
void CommandManager::ExecuteCmd(vk::Queue queue, RecordCmdFunc func)
{
    std::shared_ptr<CommandBuffer> cmd(new CommandBuffer);
    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    if (func)
        func(cmd->get_handle());
    // cmd->Get_handle().pipelineBarrier(vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::DependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const vk::MemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const vk::BufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const vk::ImageMemoryBarrier* pImageMemoryBarriers);

    // cmd->Get_handle().pipelineBarrier(vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::DependencyFlags dependencyFlags, const vk::ArrayProxy<const vk::MemoryBarrier> &memoryBarriers, const vk::ArrayProxy<const vk::BufferMemoryBarrier> &bufferMemoryBarriers, const vk::ArrayProxy<const vk::ImageMemoryBarrier> &imageMemoryBarriers);
    cmd->End();
    submit_Cmd(queue, cmd);
}
void CommandManager::submit_Cmd(vk::Queue queue,
                                std::shared_ptr<CommandBuffer> cmd)
{
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmd->get_handle());
    queue.submit(submitInfo);
      queue.waitIdle();

}

}