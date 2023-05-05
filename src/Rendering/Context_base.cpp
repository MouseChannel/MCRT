#include "Rendering/Context_base.hpp"
#include "Rendering/Render_Target/Render_Target.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/RenderPass.hpp"

namespace MCRT {
// Context_base::Context_base()
// {
// }
void Context_base::prepare()
{
    fill_render_targets();
    Prepare_RenderPass();
    Prepare_Framebuffer();
    prepare_descriptorset();
    prepare_pipeline();
}
void Context_base::Prepare_RenderPass()
{
    Get_render_pass().reset(new RenderPass);
    auto render_pass = Get_render_pass();
    for (int i = 0; i < Get_render_targets().size(); i++) {
        auto& render_target = Get_render_targets()[i];
        auto& subpass = render_pass->Get_Subpass();
        render_target->Make_Subpass(i, subpass);
        render_pass->Add_Attachment_description(render_target->Get_attachment_description());
    }
    render_pass->Build();
}
std::shared_ptr<CommandBuffer> Context_base::Begin_Record_Command_Buffer()
{
    auto cmd = get_commandbuffer();
    cmd->Reset();
    auto render_pass = Get_render_pass();
    vk::RenderPassBeginInfo render_pass_begin_info;
    vk::Rect2D rect;

    // auto extent = Context::Get_Singleton()->get_swapchain()->Get_Extent2D();
    vk::Extent2D extent;
    extent.setHeight(800)
        .setWidth(800);
    rect.setOffset({ 0, 0 })
        .setExtent(extent);

    std::vector<vk::ClearValue> clear_values;

    for (auto& i : Get_render_targets()) {
        clear_values.push_back(i->Get_clearcolor());
    }

    render_pass_begin_info.setRenderPass(render_pass->get_handle())
        .setRenderArea(rect)
        .setFramebuffer(get_framebuffer()->get_handle())
        .setClearValues(clear_values);
    // auto pipeline = Context::Get_Singleton()->Get_Pipeline();
    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd->BeginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    return cmd;
}
}