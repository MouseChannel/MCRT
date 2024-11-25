#include "Helper/CommandManager.hpp"
#include "Wrapper/CommandBuffer.hpp"

namespace MCRT {
std::shared_ptr<CommandBuffer> CommandManager::Get_one_time_cmd()
{
    std::shared_ptr<CommandBuffer> cmd(new CommandBuffer);
    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return cmd;
}
void CommandManager::ExecuteCmd(vk::Queue queue, RecordCmdFunc func)
{
    auto cmd = Get_one_time_cmd();

    if (func)
        func(cmd->get_handle());

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