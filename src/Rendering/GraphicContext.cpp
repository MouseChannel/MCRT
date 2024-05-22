#include "Rendering/GraphicContext.hpp"
#include "Helper/CommandManager.hpp"
// #include "Helper/DescriptorManager.hpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/ComputeContext.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Rendering/Render_Frame.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Rendering/Render_Target/Gbuffer_RenderTarget.hpp"
#include "Rendering/Render_Target/Resover_Render_Target.hpp"
#include "Rendering/Render_Target/SwapChainTarget.hpp"

#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Fence.hpp"
#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Semaphore.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/SwapChain.hpp"
#include <Helper/Camera.hpp>

namespace MCRT {

GraphicContext::GraphicContext(std::shared_ptr<Device> device)
    : m_device(device)
{

    if (enable_swapchain) {

        Context::Get_Singleton()->set_swapchain(std::make_shared<SwapChain>());

        m_swapchain = Context::Get_Singleton()->get_swapchain();
        render_frame_count = m_swapchain->Get_Swapchain_Image_size();
    }
    for (int i = 0; i < render_frame_count; i++) {
        fences.emplace_back(new Fence);
    }
}

GraphicContext::~GraphicContext()
{
}

void GraphicContext::fill_render_targets()
{
    auto count { enable_swapchain ? render_frame_count : 1 };

    if (all_rendertargets.empty()) {

        all_rendertargets.clear();
        all_rendertargets.resize(count);
    }
    { // recreate renderTarget
        for (int i = 0; i < all_rendertargets.size(); i++) {
            auto cur_swapchain_img = m_swapchain->Get_Images()[i];
            for (int j = 0; j < all_rendertargets[i].size(); j++) {
                auto& cur = all_rendertargets[i][j];
                cur->recreate(i);
            }
        }
    }
}

int GraphicContext::AddColorRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        // auto swapchain_image = m_swapchain->Get_Images()[i];
        // AddRenderTarget(Color_RenderTarget::Create(swapchain_image));
        // all_rendertargets[i].push_back(Color_RenderTarget::Create( ));
        all_rendertargets[i].emplace_back(new Color_RenderTarget());
    }
    colorAttachmentIndex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}

int GraphicContext::AddSwapchainRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        auto swapchain_image = m_swapchain->Get_Images()[i];
        // AddRenderTarget(Color_RenderTarget::Create(swapchain_image));
        // all_rendertargets[i].push_back(Color_RenderTarget::Create(swapchain_image));
        all_rendertargets[i].emplace_back(new SwapChainTarget(swapchain_image));
    }
    swapChainAttachmentindex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddDepthRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        // all_rendertargets[i].push_back(Depth_RenderTarget::Create());
        all_rendertargets[i].emplace_back(new Depth_RenderTarget);
    }
    depthAttachmentIndex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddResolveRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        // all_rendertargets[i].push_back(Resolve_RenderTarget::Create());
        all_rendertargets[i].emplace_back(new Resolve_RenderTarget);
    }
    resolveAttachmentindex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddGbufferRenderTarget(vk::Format format)
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        // auto swapchain_image = m_swapchain->Get_Images()[i];
        // all_rendertargets[i].push_back(GBuffer_RenderTarget::Create(format));
        all_rendertargets[i].emplace_back(new GBuffer_RenderTarget);
    }
    return all_rendertargets[0].size() - 1;
}

void GraphicContext::Prepare_Framebuffer()
{
    auto count { enable_swapchain ? render_frame_count : 1 };
    render_frames.resize(count);
    for (auto i { 0 }; i < count; i++) {
        render_frames[i].reset(new RenderFrame(m_renderpass, all_rendertargets[i]));
    }
}
// void GraphicPass::SetSubPass(std::vector<std::shared_ptr<BasePass>> subpasses, std::vector<vk::SubpassDependency> dependencies)
//{
//     m_renderpass->Build(subpasses, dependencies);
// }

void GraphicContext::Prepare_RenderPass()
{
    Get_render_pass().reset(new RenderPass);
    auto& render_pass = Get_render_pass();
    for (int i = 0; i < Get_render_targets().size(); i++) {
        auto& render_target = Get_render_targets()[i];
        //        render_target->Make_Subpass(i, render_pass);
        render_pass->Add_Attachment_description(render_target->Get_attachment_description());
    }
    for (auto& subpass : subpasses) {
    }
    render_pass->Build({ subpasses }, m_subpass_dependencies);
    //    render_pass->Build();
    //    Context::Get_Singleton()->get_debugger()->set_name(render_pass, "main renderpass");
}

void GraphicContext::prepare()
{
    fill_render_targets();
}

void GraphicContext::post_prepare()
{
    Prepare_RenderPass();
    Prepare_Framebuffer();
    for (auto& subpass : subpasses) {
        subpass->post_prepare();
    }

    command_buffer.reset(new CommandBuffer);
}

std::shared_ptr<CommandBuffer> GraphicContext::BeginFrame()
{
    auto cur_semaphone = Get_cur_render_semaphore()->get_handle();
    // Context::Get_Singleton()->get_debugger()->set_handle_name(cur_semaphone, "semaphone" + std::to_string(current_index));
    if (has_inited) {
        cur_semaphone = Get_RenderFrame((current_index + 1) % render_frame_count)->Get_render_semaphore()->get_handle();

    } else {
        cur_semaphone = Get_cur_render_semaphore()->get_handle();
    }
    has_inited = true;
    auto result = m_device->get_handle().acquireNextImageKHR(
        m_swapchain->get_handle(),
        std::numeric_limits<uint64_t>::max(),
        cur_semaphone);

    if (result.result != vk::Result::eSuccess) {
        std::cout << "render fail" << std::endl;
    }
    current_index = result.value;
    return Begin_Record_Command_Buffer();
}

std::shared_ptr<CommandBuffer> GraphicContext::Begin_Record_Command_Buffer()
{

    auto& cmd = command_buffer;

    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    return cmd;
}
void GraphicContext::Begin_RenderPass(std::shared_ptr<CommandBuffer> cmd)
{
    auto render_pass = Get_render_pass();
    vk::RenderPassBeginInfo render_pass_begin_info;
    std::vector<vk::ClearValue> clear_values;

    for (auto& i : Get_render_targets()) {
        clear_values.push_back(i->Get_clearcolor());
    }

    render_pass_begin_info.setRenderPass(render_pass->get_handle())
        .setRenderArea(vk::Rect2D()
                           .setOffset({ 0, 0 })
                           .setExtent(Context::Get_Singleton()
                                          ->get_extent2d()))
        .setFramebuffer(Get_RenderFrame(current_index)
                            ->Get_Framebuffer()
                            ->get_handle())
        .setClearValues(clear_values);
    cmd->BeginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
}

//---
void GraphicContext::End_Record_Command_Buffer()
{

    command_buffer->EndRenderPass();
    command_buffer->End();
}

void GraphicContext::Submit()
{
    End_Record_Command_Buffer();
    auto graphic_queue = m_device->Get_Graphic_queue();
    auto cur = current_index;
    vk::SubmitInfo submit_info;
    const vk::PipelineStageFlags wait_mask { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submit_info.setCommandBuffers(command_buffer->get_handle())
        .setWaitSemaphores(Get_cur_render_semaphore()->get_handle())
        .setWaitDstStageMask(wait_mask)
        .setSignalSemaphores(Get_cur_present_semaphore()->get_handle());

    auto fence = Get_cur_fence()->get_handle();

    graphic_queue.submit(submit_info, fence);
}

void GraphicContext::EndFrame()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto fence_res = m_device->get_handle().waitForFences(
        Get_cur_fence()->get_handle(),
        true,
        std::numeric_limits<uint64_t>::max());
    if (fence_res != vk::Result::eSuccess) {
        std::cout << "Wait fence fail" << std::endl;
    }

    m_device->get_handle().resetFences(Get_cur_fence()->get_handle());

#else
    auto fence_res = m_device->get_handle().waitForFences(
        Get_cur_fence()->get_handle(),
        true,
        std::numeric_limits<uint64_t>::max());
    if (fence_res != vk::Result::eSuccess) {
        std::cout << "Wait fence fail" << std::endl;
    }

    m_device->get_handle().resetFences(Get_cur_fence()->get_handle());
#endif

    vk::PresentInfoKHR present_info;
    present_info.setImageIndices(current_index)
        .setSwapchains(m_swapchain->get_handle())
        .setWaitSemaphores(Get_cur_present_semaphore()->get_handle());

    auto present_queue = m_device->Get_present_queue();

    auto present_result = present_queue.presentKHR(&present_info);

    if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR) {

        re_create();
    }
}

void GraphicContext::re_create()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    m_device->get_handle().waitIdle();
    Context::Get_Singleton()->get_swapchain().reset(new SwapChain);
    m_swapchain = Context::Get_Singleton()->get_swapchain();
    auto extent = m_swapchain->Get_Extent2D();

    Context::Get_Singleton()->set_extent2d(extent.width, extent.height);
    fill_render_targets();
    Prepare_Framebuffer();
    //    Context::Get_Singleton()
    //        ->get_camera()
    //        ->setPerpective(90, (float)extent.width / (float)extent.height, 0.1f, 10000);
    //---
    Context::Get_Singleton()->re_create_context();
#else
    current_index = 0;
    has_inited = false;
    m_device->get_handle().waitIdle();
    int cur_width = 0, cur_height = 0;
    glfwGetFramebufferSize(Context::Get_Singleton()->get_window()->get_handle(),
                           &cur_width,
                           &cur_height);
    while (cur_width == 0 || cur_height == 0) {
        glfwWaitEvents();
        glfwGetFramebufferSize(Context::Get_Singleton()->get_window()->get_handle(),
                               &cur_width,
                               &cur_height);
    }
    Context::Get_Singleton()->get_device()->get_handle().waitIdle();
    std::cout << cur_width << ' ' << cur_height << std::endl;
    Context::Get_Singleton()->get_swapchain().reset(new SwapChain);
    m_swapchain = Context::Get_Singleton()->get_swapchain();
    Context::Get_Singleton()->set_extent2d(cur_width, cur_height);
    fill_render_targets();
    Prepare_Framebuffer();
    Context::Get_Singleton()
        ->get_camera()
        ->setPerpectiveFOV(45.f, cur_width, cur_height, 0.001f, 10000.f);

    //---
    Context::Get_Singleton()->re_create_context();
    for (auto& i : subpasses) {
        i->recreate();
    }
#endif
}

void GraphicContext::re_create_swapchain()
{
}

void GraphicContext::AddSubPassDependency(vk::SubpassDependency dependency)
{
    m_subpass_dependencies.push_back(dependency);
}
}
