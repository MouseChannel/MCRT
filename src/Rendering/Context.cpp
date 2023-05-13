#include "Rendering/Context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
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

    Obj_loader::load_model("D:/MoChengRT/assets/mocheng.obj");

    m_camera.reset(new Camera);
    m_camera->init();
    contexts.push_back(std::shared_ptr<RenderContext> { new RenderContext(m_device) });
    contexts.push_back(std::shared_ptr<RT_Context> { new RT_Context(m_device) });

    contexts[1]->prepare();
    contexts[0]->prepare();
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
    // get_device()->get_handle().waitIdle();
    auto& rt_context = contexts[1];
    auto cmd = rt_context->BeginFrame();
    {

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
    // get_device()->get_handle().waitIdle();
    auto& render_context = contexts[0];
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
std::shared_ptr<CommandBuffer> Context::Begin_Frame()
{
    m_camera->move_update();
    BeginRTFrame();
    EndRTFrame();
    return BeginGraphicFrame();
}
void Context::EndFrame()
{
    EndGraphicFrame();
}
} // namespace MCRT