#include "Rendering/ComputeContext.hpp"
// #include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
// #include "Rendering/RaytracingPass.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"

namespace MCRT {
std::shared_ptr<CommandBuffer> ComputeContext::get_commandbuffer()
{
    return m_command_buffer;
}

void ComputeContext::prepare()
{
    m_command_buffer.reset(new CommandBuffer);
}
void ComputeContext::post_prepare()
{
}
std::shared_ptr<CommandBuffer> ComputeContext::BeginFrame()
{
    m_command_buffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return m_command_buffer;
}
void ComputeContext::set_barrier(std::shared_ptr<CommandBuffer> cmd)
{
}

void ComputeContext::Submit()
{
    m_command_buffer->End();

    auto queue = Context::Get_Singleton()->get_device()->Get_Graphic_queue();
    vk::SubmitInfo submit_info;
    submit_info.setCommandBuffers(m_command_buffer->get_handle());
    queue.submit(submit_info);
}
void ComputeContext::EndFrame()
{
    m_command_buffer->End();
}
void ComputeContext::re_create()
{
}
}