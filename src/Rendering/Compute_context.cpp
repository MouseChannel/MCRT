#include "Rendering/Compute_context.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/RT_Context.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Image.hpp"
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

std::shared_ptr<Image> Compute_Context::get_out_image()
{
    return m_out_image;
}
void Compute_Context::create_uniform_buffer()
{

    test_data = UniformManager::make_uniform({ test_Compute { .a = 1, .b = 2 } },
                                             vk::ShaderStageFlagBits::eCompute,
                                             vk::DescriptorType::eUniformBuffer);
}

void Compute_Context::prepare_descriptorset()
{
    // Descriptor_Manager::Get_Singleton()->Make_DescriptorSet(test_data, 0, Descriptor_Manager::Compute);
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(m_out_image,
                             0,
                             vk::DescriptorType::eStorageImage,
                             vk::ShaderStageFlagBits::eCompute,
                             Descriptor_Manager::Compute);

    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Compute);
    Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet(Descriptor_Manager::Compute);
}
void Compute_Context::prepare_pipeline()
{
    m_compute_pipeline.reset(new Compute_Pipeline);
}
void Compute_Context::prepare()
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
    create_uniform_buffer();
    prepare_descriptorset();
    prepare_pipeline();
    m_command_buffer.reset(new CommandBuffer);
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

    cmd->get_handle()
        .bindDescriptorSets(vk::PipelineBindPoint ::eCompute,
                            m_compute_pipeline->get_layout(),
                            0,
                            { Descriptor_Manager::Get_Singleton()
                                  ->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)
                                  ->get_handle()[0],
                              Descriptor_Manager::Get_Singleton()
                                  ->get_DescriptorSet(Descriptor_Manager::Compute)
                                  ->get_handle()[0] },
                            {});
    cmd->get_handle()
        .bindPipeline(vk::PipelineBindPoint::eCompute,
                      m_compute_pipeline->get_handle());

    vk::MemoryBarrier2 memory_barrier, memory_barrier2, memory_barrier3, memory_barrier4;
    memory_barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderWrite)
        .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    memory_barrier2.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
        .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderWrite);
    memory_barrier3.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderWrite)
        .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    memory_barrier4.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
        .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
        .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead | vk::AccessFlagBits2::eShaderSampledRead);
    std::vector<vk::MemoryBarrier2> barriers {
        memory_barrier,
        memory_barrier2,
        memory_barrier3,
        memory_barrier4
    };
    cmd->get_handle()
        .dispatch(800, 749, 1);
    auto pushConstantsss = Context::Get_Singleton()->get_enable_filter();
    auto push_contants = PushContant_Compute {
        .frame = Context::Get_Singleton()->get_rt_context()->get_cur_frame_id(),
        .open_filter = Context::Get_Singleton()->get_enable_filter()
    };
    cmd->get_handle()
        .pushConstants<PushContant_Compute>(get_pipeline()->get_layout(), vk::ShaderStageFlagBits::eCompute, 0, push_contants);

    Context::Get_Singleton()->get_debugger()->set_name(cmd, "compute command_buffer");
    auto ee = get_out_image();
    // cmd->get_handle().setCheckpointNV(testcheck);
    cmd->get_handle()
        .pipelineBarrier2(vk::DependencyInfo().setMemoryBarriers(barriers));
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