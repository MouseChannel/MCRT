#include "Rendering/Compute_context.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"

namespace MCRT {

std::shared_ptr<Pipeline_base> Compute_Context::get_pipeline()
{
    return m_compute_pipeline;
}
std::shared_ptr<CommandBuffer> Compute_Context::get_commandbuffer()
{
    return m_command_buffer;
}

void Compute_Context::create_uniform_buffer()
{

    test_data = UniformManager::make_uniform({ test_Compute { .a = 1, .b = 2 } },
                                             vk::ShaderStageFlagBits::eCompute,
                                             vk::DescriptorType::eUniformBuffer);
}

void Compute_Context::prepare_descriptorset()
{
    Descriptor_Manager::Get_Singleton()->Make_DescriptorSet(test_data, 0, Descriptor_Manager::Compute);
    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Compute);
    Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet(Descriptor_Manager::Compute);
}
void Compute_Context::prepare_pipeline()
{
    m_compute_pipeline.reset(new Compute_Pipeline);
}
void Compute_Context::prepare()
{
    create_uniform_buffer();
    prepare_descriptorset();
    prepare_pipeline();
    m_command_buffer.reset(new CommandBuffer);
}

std::shared_ptr<CommandBuffer> Compute_Context::BeginFrame()
{
    m_command_buffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return m_command_buffer;
}

void Compute_Context::record_command(std::shared_ptr<CommandBuffer> cmd)
{
    cmd->get_handle()
        .bindDescriptorSets(vk::PipelineBindPoint ::eCompute, m_compute_pipeline->get_layout(), 0, { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute)->get_handle()[0] }, {});
    cmd->get_handle()
        .bindPipeline(vk::PipelineBindPoint::eCompute,
                      m_compute_pipeline->get_handle());

    cmd->get_handle()
        .dispatch(10, 1, 1);
}

void Compute_Context::Submit()
{
    m_command_buffer->End();

    auto queue = Context::Get_Singleton()->get_device()->Get_Graphic_queue();
    vk::SubmitInfo submit_info;
    submit_info.setCommandBuffers(m_command_buffer->get_handle());
    queue.submit(submit_info);
}
void Compute_Context::EndFrame()
{
    m_command_buffer->End();
}
}