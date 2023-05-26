#include "Rendering/Compute_context.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/RT_Context.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"

namespace MCRT {

std::shared_ptr<Pipeline_base> Compute_Context::get_pipeline()
{
    return m_compute_pipeline;
}
std::shared_ptr<CommandBuffer> Compute_Context::get_commandbuffer()
{
    return m_command_buffer;
}

std::shared_ptr<Image> Compute_Context::get_out_image()
{
    return m_out_image;
}

void Compute_Context::prepare_descriptorset(std::function<void()> prepare_func)
{
    prepare_func();

    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Compute);
    Descriptor_Manager::Get_Singleton()->update_descriptor_set(Descriptor_Manager::Compute);
}
void Compute_Context::prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules)
{

    m_compute_pipeline.reset(new Compute_Pipeline(shader_modules));
}
void Compute_Context::prepare(std::vector<std::shared_ptr<ShaderModule>> shader_modules)
{
    m_out_image.reset(new Image(800,
                                749,
                                vk::Format::eR32G32B32A32Sfloat,
                                vk::ImageType::e2D,
                                vk::ImageTiling::eOptimal,
                                vk::ImageUsageFlagBits::eStorage |
                                    vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                                vk::ImageAspectFlagBits::eColor,
                                vk::SampleCountFlagBits::e1));

    m_out_image->SetImageLayout(vk::ImageLayout::eGeneral, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe);

    // prepare_descriptorset();
    // prepare_pipeline(shader_modules);
    m_command_buffer.reset(new CommandBuffer);
}
void Compute_Context::post_prepare()
{
}
std::shared_ptr<CommandBuffer> Compute_Context::BeginFrame()
{
    // m_command_buffer->Reset();
    m_command_buffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return m_command_buffer;
}
void Compute_Context::set_barrier(std::shared_ptr<CommandBuffer> cmd)
{
}

void Compute_Context::record_command(std::shared_ptr<CommandBuffer> cmd)
{
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