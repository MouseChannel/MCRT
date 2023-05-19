#include "Rendering/Render_Context.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Frame.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Rendering/Render_Target/Final_RenderTarget.hpp"
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

#include "Rendering/Render_Target/MultiSampler_Render_Target.hpp"

namespace MCRT {

RenderContext::RenderContext(std::shared_ptr<Device> device)
    : m_device(device)

{

    if (enable_swapchain) {

        // m_swapchain = Context::Get_Singleton()->get_swapchain();

        m_swapchain.reset(new SwapChain);
        Context::Get_Singleton()->set_swapchain(m_swapchain);
        render_frame_count = m_swapchain->Get_Swapchain_Image_size();
    }
    for (int i = 0; i < render_frame_count;
         i++) {
        fences.emplace_back(new Fence);
    }

    // command_buffer.reset(new CommandBuffer);
    // command_buffer = Context::Get_Singleton()->get_compute_context()->get_commandbuffer();
    int s = 0;
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
std::shared_ptr<Pipeline_base> RenderContext::get_pipeline()
{
    return m_graphic_pipeline;
}
void RenderContext::fill_render_targets()
{
    auto count { enable_swapchain ? 3 : 1 };
    all_rendertargets.resize(count);
    for (auto i { 0 }; i < count; i++) {

        auto swapchain_image_handle = m_swapchain->Get_Swapchain_Images()[i];
        std::shared_ptr<Image> swapchain_image {
            new Image(swapchain_image_handle,
                      vk::ImageLayout::eColorAttachmentOptimal,
                      m_swapchain->Get_Format(),
                      vk::ImageAspectFlagBits::eColor)
        };
        all_rendertargets[i].emplace_back(Color_RenderTarget::Create(swapchain_image));
    }
}
void RenderContext::Prepare_Framebuffer()
{
    auto count { enable_swapchain ? 3 : 1 };
    for (auto i { 0 }; i < count; i++) {
        render_frames.emplace_back(new RenderFrame(m_renderpass,
                                                   all_rendertargets[i]));
    }
}
void RenderContext::Prepare_RenderPass()
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
void RenderContext::prepare_descriptorset(std::function<void()> prepare)
{
    prepare();

    Descriptor_Manager::Get_Singleton()
        ->CreateDescriptorPool(Descriptor_Manager::Graphic);
    Descriptor_Manager::Get_Singleton()
        ->CreateUpdateDescriptorSet(Descriptor_Manager::Graphic);
}
void RenderContext::prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules)
{

    m_graphic_pipeline.reset(new Graphic_Pipeline(shader_modules));

    vk::VertexInputBindingDescription vert_position_binding;
    vert_position_binding
        .setBinding(0)
        .setInputRate(vk::VertexInputRate ::eVertex)
        .setStride(sizeof(float) * 3);
    vk::VertexInputAttributeDescription vertex_position_attr;
    vertex_position_attr.setBinding(0)
        .setFormat(vk::Format ::eR32G32B32Sfloat)
        .setLocation(0)
        .setOffset(0);

    vk::VertexInputBindingDescription vert_uv_binding;
    vert_uv_binding
        .setBinding(1)
        .setInputRate(vk::VertexInputRate ::eVertex)
        .setStride(sizeof(float) * 2);

    vk::VertexInputAttributeDescription vertex_uv_attr;
    vertex_uv_attr.setBinding(1)
        .setFormat(vk::Format ::eR32G32Sfloat)
        .setLocation(1)
        .setOffset(0);
    std::vector<vk::VertexInputBindingDescription> binds {
        vert_position_binding,
        vert_uv_binding
    };
    std::vector<vk::VertexInputAttributeDescription> attrs {
        vertex_position_attr,
        vertex_uv_attr
    };
    m_graphic_pipeline->Make_VertexInput(binds, attrs);
    m_graphic_pipeline->Make_VertexAssembly();
    m_graphic_pipeline->Make_viewPort();
    m_graphic_pipeline->Make_MultiSample();
    m_graphic_pipeline->Make_Resterization();
    m_graphic_pipeline->Make_attach();
    m_graphic_pipeline->Make_Blend();
    m_graphic_pipeline->Make_DepthTest();

    m_graphic_pipeline->Build_Pipeline(Context::Get_Singleton()->get_graphic_context()->Get_render_pass());
}
void RenderContext::prepare(std::vector<std::shared_ptr<ShaderModule>> shader_modules)
{
    fill_render_targets();
    Prepare_RenderPass();
    Prepare_Framebuffer();

    // command_buffer = Context::Get_Singleton()->get_compute_context()->get_commandbuffer();
    command_buffer.reset(new CommandBuffer);
}
void RenderContext::post_prepare()
{
}
std::shared_ptr<Framebuffer>& RenderContext::get_framebuffer()
{
    return Get_RenderFrame(current_index)->Get_Framebuffer();
}

std::shared_ptr<CommandBuffer> RenderContext::BeginFrame()
{
    //
    auto result = m_device->get_handle().acquireNextImageKHR(
        m_swapchain->get_handle(),
        std::numeric_limits<uint64_t>::max(),
        Get_cur_render_semaphore()->get_handle());

    if (result.result != vk::Result::eSuccess) {
        std::cout << "render fail" << std::endl;
    }
    current_index = result.value;
    return Begin_Record_Command_Buffer();
}

std::shared_ptr<CommandBuffer> RenderContext::Begin_Record_Command_Buffer()
{

    // command_buffer = Context::Get_Singleton()->get_compute_context()->get_commandbuffer();
    auto& cmd = command_buffer;

    // cmd->Reset();
    auto render_pass = Get_render_pass();
    vk::RenderPassBeginInfo render_pass_begin_info;
    vk::Rect2D rect;

    // auto extent = Context::Get_Singleton()->get_swapchain()->Get_Extent2D();
    vk::Extent2D extent;
    extent.setHeight(749)
        .setWidth(800);
    rect.setOffset({ 0, 0 })
        .setExtent(extent);

    std::vector<vk::ClearValue> clear_values;

    for (auto& i : Get_render_targets()) {
        clear_values.push_back(i->Get_clearcolor());
    }

    render_pass_begin_info.setRenderPass(render_pass->get_handle())
        .setRenderArea(rect)
        .setFramebuffer(Get_RenderFrame(current_index)
                            ->Get_Framebuffer()
                            ->get_handle())
        .setClearValues(clear_values);

    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd->BeginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    return cmd;
}
void RenderContext::record_command(std::shared_ptr<CommandBuffer> cmd)
{
    auto cmd_handle = cmd->get_handle();
    // cmd_handle.bindIndexBuffer(index_buffer->get_handle(), 0, vk::IndexType ::eUint32);
    // cmd_handle.bindVertexBuffers(0, {
    //                                     vertex_buffer->get_handle(),
    //                                     uv_buffer->get_handle(),
    //                                 },
    //                              { 0, 0 });

    // cmd_handle.bindPipeline(vk::PipelineBindPoint ::eGraphics, get_pipeline()->get_handle());
    // cmd_handle.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
    //                               get_pipeline()->get_layout(),
    //                               0,
    //                               { //   Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute)->get_handle()[0],
    //                                 Descriptor_Manager::Get_Singleton()
    //                                     ->get_DescriptorSet(Descriptor_Manager::Graphic)
    //                                     ->get_handle() },

    //                               {});
    // cmd_handle.drawIndexed(Model::models[0]->get_vertex_count(), 1, 0, 0, 0);

    // vk::MemoryBarrier2 memory_barrier;

    // memory_barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
    //     .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
    //     .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
    //     .setDstAccessMask(vk::AccessFlagBits2::eShaderRead);
    // cmd_handle.pipelineBarrier2(vk::DependencyInfo().setMemoryBarriers(memory_barrier));
    Context::Get_Singleton()->get_debugger()->set_name(cmd, "render command_buffer");
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
    submit_info.setCommandBuffers(command_buffer->get_handle())
        .setWaitSemaphores(Get_cur_render_semaphore()->get_handle())
        .setWaitDstStageMask(wait_mask)
        .setSignalSemaphores(Get_cur_present_semaphore()->get_handle());
    graphic_queue.submit(submit_info, Get_cur_fence()->get_handle());
}
void RenderContext::EndFrame()
{
    auto fence_res = m_device->get_handle().waitForFences(
        Get_cur_fence()->get_handle(),
        true,
        std::numeric_limits<uint64_t>::max());
    if (fence_res != vk::Result::eSuccess) {
        std::cout << "Wait fence fail" << std::endl;
    }

    m_device->get_handle().resetFences(Get_cur_fence()->get_handle());

    vk::PresentInfoKHR present_info;
    present_info.setImageIndices(current_index)
        .setSwapchains(m_swapchain->get_handle())
        .setWaitSemaphores(Get_cur_present_semaphore()->get_handle());

    auto present_queue = m_device->Get_present_queue();

    auto present_result = present_queue.presentKHR(present_info);

    if (present_result != vk::Result::eSuccess) {
        std::cout << "present fail" << std::endl;
    }
    current_frame++;
    current_frame %= render_frame_count;
}
} // namespace MoCheng3D
