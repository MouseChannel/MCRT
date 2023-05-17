#include "Rendering/Context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Ray_Tracing/RT_Manager.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include "Wrapper/Window_Surface.hpp"
#include "vulkan/vulkan_to_string.hpp"

namespace MCRT {

void Context::init(std::shared_ptr<Window> window)
{
    m_window = window;
    m_instance.reset(new Instance);
    m_surface.reset(new Surface);
    m_device.reset(new Device);
    m_command_pool.reset(new CommandPool);
    // m_command_buffer.reset(new CommandBuffer);
    // m_swapchain.reset(new SwapChain);
    // m_renderpass.reset(new RenderPass);
    m_debugger.reset(new Debugger);
    m_sampler.reset(new Sampler);
    // m_debugger->set_buffer_name(m_command_pool, "rer");

    Obj_loader::load_model("D:/MoChengRT/assets/mocheng.obj");

    m_camera.reset(new Camera);
    m_camera->init();
    contexts.resize(3);
    contexts[Context_index::Compute] = std::shared_ptr<Compute_Context> { new Compute_Context };
    contexts[Context_index::Ray_tracing] = std::shared_ptr<RT_Context> { new RT_Context(m_device) };
    contexts[Context_index::Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };

    contexts[Ray_tracing]->prepare();
    contexts[Compute]->prepare();
    contexts[Graphic]->prepare();
}
std::shared_ptr<RenderPass> Context::get_renderpass()
{
    return get_graphic_context()->Get_render_pass();
}

std::shared_ptr<CommandBuffer> Context::BeginRTFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& rt_context = contexts[Ray_tracing];
    auto cmd = rt_context->BeginFrame();
    {
        cmd->get_handle().setCheckpointNV(12);
        rt_context->record_command(cmd);
    }
    return cmd;
}

void Context::EndRTFrame()
{
    auto& rt_context = contexts[Ray_tracing];
    rt_context->Submit();
}
std::shared_ptr<CommandBuffer> Context::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& render_context = contexts[Graphic];
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        render_context->record_command(cmd);
    }

    return cmd;
}

void Context::EndGraphicFrame()
{
    auto& m_render_context = contexts[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}
std::shared_ptr<CommandBuffer> Context::BeginComputeFrame()
{

    // compute_context->record_command(cmd);
    auto& compute_context = contexts[2];
    std::shared_ptr<CommandBuffer> cmd = compute_context->BeginFrame();
    {
        compute_context->record_command(cmd);
    }
    return cmd;
}
void Context::EndComputeFrame()
{
    auto& compute_context = contexts[2];
    compute_context->Submit();
}
std::shared_ptr<CommandBuffer> Context::Begin_Frame()
{
    m_camera->move_update();
    auto cmd = BeginRTFrame();
    EndRTFrame();
    BeginComputeFrame();
    EndComputeFrame();

    return BeginGraphicFrame();
}
void Context::EndFrame()
{
    auto ee = get_device()->Get_Graphic_queue().getCheckpointData2NV();
    for (auto& i : ee) {
        auto dd = vk::to_string(i.stage);
        auto rr = *(Vertex*)i.pCheckpointMarker;
        int eeee = 0;
    }
    EndGraphicFrame();
}
} // namespace MCRT