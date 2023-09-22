#include "Rendering/Render_Context.hpp"
#include "Rendering/Render_Frame.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Fence.hpp"
#include "Wrapper/Pipeline.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Semaphore.hpp"
#include "Wrapper/SwapChain.hpp"

#include "Rendering/Render_Target/Final_RenderTarget.hpp"

#include "Rendering/Render_Target/MultiSampler_Render_Target.hpp"

namespace MoCheng3D {

RenderContext::RenderContext(std::shared_ptr<Device> device)
    : m_device(device)

{
    m_swapchain = Context::Get_Singleton()->Get_SwapChain();
    for (int i = 0; i < Context::Get_Singleton()->Get_SwapChain()->Get_Swapchain_Image_size();
         i++) {
        fences.emplace_back(new Fence);
    }

    command_buffer.reset(new CommandBuffer);
}

RenderContext::~RenderContext()
{
    command_buffer.reset();
    fences.clear();
    m_renderpass.reset();
    render_frames.clear();
    m_swapchain.reset();
    m_device.reset();
}

void RenderContext::Prepare()
{

    for (int i = 0; i < m_swapchain->Get_Swapchain_Image_size(); i++) {

        // todo need add multisampler stuff
        std::shared_ptr<Image> swapchain_image { new Image(
            m_swapchain->Get_Swapchain_Images()[i],
            vk::ImageLayout::eColorAttachmentOptimal, m_swapchain->Get_Format(),
            vk::ImageAspectFlagBits::eColor) };

        std::vector<std::shared_ptr<RenderTarget>>
            render_targets;

        render_targets.emplace_back(Final_RenderTarget::Create(swapchain_image));

        render_targets.emplace_back(MultiSampler_RenderTarget::Create());
        render_targets.emplace_back(Depth_RenderTarget::Create());
        if (!m_renderpass)
            Prepare_RenderPass(render_targets);
        render_frames.emplace_back(new RenderFrame(m_device, render_targets));
    }
}

void RenderContext::Prepare_RenderPass(std::vector<std::shared_ptr<RenderTarget>>& render_targets)
{

    m_renderpass.reset(new RenderPass);
    for (int i = 0; i < render_targets.size(); i++) {
        auto& render_target = render_targets[i];

        render_target->Make_Subpass(i, m_renderpass->Get_Subpass());
        m_renderpass->Add_Attachment_description(render_target->Get_attachment_description());
    }
    m_renderpass->Build();
}

std::shared_ptr<CommandBuffer> RenderContext::BeginFrame()
{
    //
    auto result = m_device->Get_handle().acquireNextImageKHR(
        m_swapchain->Get_handle(), std::numeric_limits<uint64_t>::max(),
        Get_cur_render_semaphore()->Get_handle());

    if (result.result != vk::Result::eSuccess) {
        std::cout << "render fail" << std::endl;
    }
    current_index = result.value;
    return Begin_Record_Command_Buffer();
}

std::shared_ptr<CommandBuffer> RenderContext::Begin_Record_Command_Buffer()
{
    auto& cmd = command_buffer;

    cmd->Reset();
    auto render_pass = Context::Get_Singleton()->Get_RenderPass();
    vk::RenderPassBeginInfo render_pass_begin_info;
    vk::Rect2D rect;

    rect.setOffset({ 0, 0 }).setExtent( m_swapchain->Get_Extent2D() );

    std::vector<vk::ClearValue> clear_values;

    for (auto& i : render_frames[0]->Get_render_targets()) {
        clear_values.push_back(i->Get_clearcolor());
    }

    render_pass_begin_info.setRenderPass(render_pass->Get_handle())
        .setRenderArea(rect)
        .setFramebuffer(Get_RenderFrame(current_index)->Get_Framebuffer()->Get_handle())
        .setClearValues(clear_values);
    auto pipeline = Context::Get_Singleton()->Get_Pipeline();
    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd->BeginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    return cmd;
}

//---
void RenderContext::End_Record_Command_Buffer()
{

    command_buffer->EndRenderPass();
    command_buffer->End();
}
void RenderContext::Submit()
{
    End_Record_Command_Buffer();
    auto graphic_queue = m_device->Get_Graphic_queue();

    vk::SubmitInfo submit_info;
    const vk::PipelineStageFlags wait_mask { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submit_info.setCommandBuffers(command_buffer->Get_handle())
        .setWaitSemaphores(Get_cur_render_semaphore()->Get_handle())
        .setWaitDstStageMask(wait_mask)
        .setSignalSemaphores(Get_cur_present_semaphore()->Get_handle());
    graphic_queue.submit(submit_info, Get_cur_fence()->Get_handle());
}
void RenderContext::EndFrame()
{
    auto fence_res = m_device->Get_handle().waitForFences(
        Get_cur_fence()->Get_handle(), true, std::numeric_limits<uint64_t>::max());
    if (fence_res != vk::Result::eSuccess) {
        std::cout << "Wait fence fail" << std::endl;
    }

    m_device->Get_handle().resetFences(Get_cur_fence()->Get_handle());

    vk::PresentInfoKHR present_info;
    present_info.setImageIndices(current_index)
        .setSwapchains(m_swapchain->Get_handle())
        .setWaitSemaphores(Get_cur_present_semaphore()->Get_handle());

    auto present_queue = m_device->Get_present_queue();

    auto present_result = present_queue.presentKHR(present_info);

    if (present_result != vk::Result::eSuccess) {
        std::cout << "present fail" << std::endl;
    }
    current_frame++;
    current_frame %= Context::Get_Singleton()->Get_SwapChain()->Get_Swapchain_Image_size();
}
} // namespace MoCheng3D
