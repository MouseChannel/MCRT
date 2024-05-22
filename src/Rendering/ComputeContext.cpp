#include "Rendering/ComputeContext.hpp"
// #include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"

namespace MCRT {

// std::shared_ptr<Pipeline_base> ComputeContext::get_pipeline()
// {
//     return m_compute_pipeline;
// }
std::shared_ptr<CommandBuffer> ComputeContext::get_commandbuffer()
{
    return m_command_buffer;
}

// std::shared_ptr<Image> ComputeContext::get_out_image()
// {
//     return m_out_image;
// }

// void ComputeContext::prepare_descriptorset(std::function<void()> prepare_func)
// {
//     prepare_func();
//     //    get_descriptor_manager()->CreateDescriptorPool(DescriptorManager::Compute);
//     get_descriptor_manager()->update_descriptor_set(DescriptorManager::Compute);
//     //    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Compute);
//     //    Descriptor_Manager::Get_Singleton()->update_descriptor_set(Descriptor_Manager::Compute);
// }
// void ComputeContext::prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size)
// {

//     m_compute_pipeline.reset(new Compute_Pipeline(shader_modules, sets, push_constants_size));
// }
void ComputeContext::prepare()
{
    // auto extent2d = Context::Get_Singleton()->get_extent2d();
    // m_out_image.reset(new Image(extent2d.width,
    //                             extent2d.height,
    //                             vk::Format::eR32G32B32A32Sfloat,
    //                             vk::ImageType::e2D,
    //                             vk::ImageTiling::eOptimal,
    //                             vk::ImageUsageFlagBits::eStorage |
    //                                 vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
    //                             vk::ImageAspectFlagBits::eColor,
    //                             vk::SampleCountFlagBits::e8));

    // m_out_image->SetImageLayout(vk::ImageLayout::eGeneral,
    //                             vk::AccessFlagBits::eNone,
    //                             vk::AccessFlagBits::eNone,
    //                             vk::PipelineStageFlagBits::eTopOfPipe,
    //                             vk::PipelineStageFlagBits::eBottomOfPipe);

    // prepare_descriptorset();
    // prepare_pipeline(shader_modules);
    m_command_buffer.reset(new CommandBuffer);
}
void ComputeContext::post_prepare()
{
}
std::shared_ptr<CommandBuffer> ComputeContext::BeginFrame()
{
    // m_command_buffer->Reset();
    m_command_buffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return m_command_buffer;
}
void ComputeContext::set_barrier(std::shared_ptr<CommandBuffer> cmd)
{
}

// void ComputeContext::record_command(std::shared_ptr<CommandBuffer> cmd)
// {
// }

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
// void ComputeContext::update_descriptorset(std::function<void()> update_func)
//{
//     update_func();
//     get_descriptor_manager()->update_descriptor_set(DescriptorManager::Compute);
// }
}