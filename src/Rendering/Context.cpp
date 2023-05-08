#include "Rendering/Context.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Ray_Tracing/RT_Manager.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include "Wrapper/Window_Surface.hpp"

namespace MCRT {

void Context::init(std::shared_ptr<Window> window)
{
    m_window = window;
    m_instance.reset(new Instance);
    m_surface.reset(new Surface);
    m_device.reset(new Device);
    m_command_pool.reset(new CommandPool);
    m_command_buffer.reset(new CommandBuffer);
    // m_swapchain.reset(new SwapChain);
    // m_renderpass.reset(new RenderPass);
    m_debugger.reset(new Debugger);
    m_sampler.reset(new Sampler);
    // m_debugger->set_buffer_name(m_command_pool, "rer");

    m_model.reset(new Model("D:/MoChengRT/assets/model.obj",
                            "D:/MoChengRT/assets/model.png"));
    contexts.push_back(std::shared_ptr<RenderContext> { new RenderContext(m_device) });
    contexts.push_back(std::shared_ptr<RT_Context> { new RT_Context(m_device) });

    contexts[1]->prepare();
    contexts[0]->prepare();
    // for (auto& i : contexts) {
    //     i->prepare();
    // }
    // AS_Builder::Get_Singleton()->add_blas_obj(m_model);

    // AS_Builder::Get_Singleton()->build_blas();
    // AS_Builder::Get_Singleton()->build_tlas();
    // create_rt_descriptor_set();

    // m_rt_pipeline.reset(new RT_Pipeline);
    // createRt_shader_binding_table();
    // m_render_context.reset(new RenderContext(m_device));
    // m_render_context->prepare();

    // std::align(size_t align, size_t sz, void *&ptr, size_t &space)
    int a = 0;
}
std::shared_ptr<Image> Context::get_out_image()
{
    return contexts[1]->get_out_image();
}
std::shared_ptr<RenderPass> Context::get_renderpass()
{
    return contexts[Graphic]->Get_render_pass();
}

std::shared_ptr<CommandBuffer>
Context::BeginRTFrame()
{
    get_device()->get_handle().waitIdle();
    auto& rt_context = contexts[1];
    auto cmd = rt_context->BeginFrame();
    {

        vk::CommandBuffer _cmd = cmd->get_handle();
        _cmd.bindPipeline(vk::PipelineBindPoint ::eRayTracingKHR, rt_context->get_pipeline()->get_handle());
        std::vector<vk::DescriptorSet> descriptor_sets {
            // descriptor_sets.push_back(
            Descriptor_Manager::Get_Singleton()
                ->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)
                ->get_handle()[0],

            Descriptor_Manager::Get_Singleton()
                ->get_DescriptorSet(Descriptor_Manager::Global)
                ->get_handle()[0]
        };
        _cmd.bindDescriptorSets(vk::PipelineBindPoint ::eRayTracingKHR,
                                rt_context->get_pipeline()->get_layout(),
                                0,
                                descriptor_sets,
                                {});
        rt_context->record_command(cmd);
    }
    return cmd;
}

void Context::EndRTFrame()
{
    auto& rt_context = contexts[1];
    rt_context->Submit();
    // rt_context->EndFrame();
}
std::shared_ptr<CommandBuffer> Context::BeginGraphicFrame()
{
    get_device()->get_handle().waitIdle();
    auto& render_context = contexts[0];
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        render_context->record_command(cmd);
        // auto cmd_handle = cmd->get_handle();
        // cmd_handle.bindIndexBuffer(, vk::DeviceSize offset, vk::IndexType indexType)
        // cmd_handle.bindPipeline(vk::PipelineBindPoint ::eGraphics, render_context->get_pipeline()->get_handle());
        // cmd_handle.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
        //                               render_context->get_pipeline()->get_layout(),
        //                               0,
        //                               Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Graphic)->get_handle(),

        //                               {});
        // cmd_handle.draw(4, 1, 0, 0);
    }

    return cmd;
}

void Context::EndGraphicFrame()
{
    auto& m_render_context = contexts[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}
std::shared_ptr<CommandBuffer> Context::Begin_Frame()
{
    BeginRTFrame();
    EndRTFrame();
    return BeginGraphicFrame();
}
void Context::EndFrame()
{
    EndGraphicFrame();
}
} // namespace MCRT