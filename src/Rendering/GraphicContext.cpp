#include "Rendering/GraphicContext.hpp"
#include "Helper/CommandManager.hpp"

#include "Rendering/ComputeContext.hpp"

#include "Rendering/AppWindow.hpp"
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
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Semaphore.hpp"
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
        // render_frame_count = 2;
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
        all_rendertargets[i].emplace_back(new Color_RenderTarget());
    }
    colorAttachmentIndex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}

int GraphicContext::AddSwapchainRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        auto swapchain_image = m_swapchain->Get_Images()[i];
        all_rendertargets[i].emplace_back(new SwapChainTarget(swapchain_image));
    }
    swapChainAttachmentindex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddDepthRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        all_rendertargets[i].emplace_back(new Depth_RenderTarget);
    }
    depthAttachmentIndex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddResolveRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        all_rendertargets[i].emplace_back(new Resolve_RenderTarget);
    }
    resolveAttachmentindex = all_rendertargets[0].size() - 1;
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddGbufferRenderTarget(vk::Format format)
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
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

void GraphicContext::Prepare_RenderPass()
{
    Get_render_pass().reset(new RenderPass);
    auto& render_pass = Get_render_pass();
    for (int i = 0; i < Get_render_targets().size(); i++) {
        auto& render_target = Get_render_targets()[i];
        render_pass->Add_Attachment_description(render_target->Get_attachment_description());
    }

    render_pass->Build(graphicPass, m_subpass_dependencies);
}

void GraphicContext::prepare()
{
    fill_render_targets();
}

void GraphicContext::post_prepare()
{
    Prepare_RenderPass();
    Prepare_Framebuffer();

    for (int i = 0; i < render_frame_count; i++) {
        command_buffers.emplace_back(new CommandBuffer);
    }
}

std::shared_ptr<CommandBuffer> GraphicContext::BeginFrame()
{

    auto fence_res = m_device->get_handle().waitForFences(1,
                                                          &Get_cur_fence()->get_handle(),
                                                          true,
                                                          std::numeric_limits<uint64_t>::max());

    m_device->get_handle().resetFences(Get_cur_fence()->get_handle());
    auto result = m_device->get_handle()
                      .acquireNextImageKHR(m_swapchain->get_handle(),
                                           std::numeric_limits<uint64_t>::max(),
                                           Get_cur_render_semaphore()->get_handle(),
                                           VK_NULL_HANDLE,
                                           &current_index_in_swapchain);

    return Begin_Record_Command_Buffer();
}

std::shared_ptr<CommandBuffer> GraphicContext::Begin_Record_Command_Buffer()
{

    // auto& cmd = command_buffer;
    auto& cmd = command_buffers[current_index];
    // cmd->Reset();
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
        .setFramebuffer(Get_RenderFrame(current_index_in_swapchain)
                            ->Get_Framebuffer()
                            ->get_handle())
        .setClearValues(clear_values);
    cmd->BeginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
}

//---
void GraphicContext::End_Record_Command_Buffer()
{
    auto& cmd = command_buffers[current_index];
    cmd->EndRenderPass();
    cmd->End();
}

void GraphicContext::Submit()
{
    End_Record_Command_Buffer();
    auto graphic_queue = m_device->Get_Graphic_queue();
    Context::Get_Singleton()->get_debugger()->set_handle_name(graphic_queue, "graphic_queue  ");

    // auto cur = current_index;
    vk::SubmitInfo submit_info;
    const vk::PipelineStageFlags wait_mask { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submit_info.setCommandBuffers(command_buffers[current_index]->get_handle())
        .setWaitSemaphores(Get_cur_render_semaphore()->get_handle())
        .setWaitDstStageMask(wait_mask)
        .setSignalSemaphores(Get_cur_present_semaphore()->get_handle());
    // .setSignalSemaphores(Get_cur_render_semaphore()->get_handle());
    // m_device->get_handle().resetFences(Get_cur_fence()->get_handle());
    graphic_queue.submit(submit_info, Get_cur_fence()->get_handle());
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

#endif

    vk::PresentInfoKHR present_info;
    present_info.setImageIndices(current_index_in_swapchain)
        .setSwapchains(m_swapchain->get_handle())
        .setWaitSemaphores(Get_cur_present_semaphore()->get_handle());

    auto present_queue = m_device->Get_present_queue();

    auto present_result = present_queue.presentKHR(&present_info);

    if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR) {

        re_create();
    }
    current_index = (current_index + 1) % render_frame_count;
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
    // current_index = 0;
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

    recreate_descriptorSet();
    Context::Get_Singleton()->re_create_context();

#endif
}

void GraphicContext::recreate_descriptorSet()
{
    for (auto descriptorSet : descriptorSets) {
        for (auto [binding, old] : descriptorSet->m_descriptorTargetMap) {
            if (old->get_type() == vk::DescriptorType::eInputAttachment) {
                descriptorSet->AddInputImageDescriptorTarget(all_rendertargets, descriptorSet->m_attachent_bind_map[binding], binding);
            }
        }
    }
    for (auto i : descriptorSets) {

        i->Update();
    }
}
void GraphicContext::re_create_swapchain()
{
}

void GraphicContext::AddSubPassDependency(vk::SubpassDependency dependency)
{
    m_subpass_dependencies.push_back(dependency);
}
}
