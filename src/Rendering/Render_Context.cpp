#include "Rendering/Render_Context.hpp"
#include "Helper/DescriptorManager.hpp"
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
void RenderContext::prepare_descriptorset()
{
    // todo descriptorset_layout

    Descriptor_Manager::Get_Singleton()->Make_DescriptorSet(Context::Get_Singleton()->get_out_image(),
                                                            0,
                                                            vk::DescriptorType ::eCombinedImageSampler,
                                                            vk::ShaderStageFlagBits::eFragment,
                                                            Descriptor_Manager::Graphic);
    Descriptor_Manager::Get_Singleton()
        ->CreateDescriptorPool(Descriptor_Manager::Graphic);
    Descriptor_Manager::Get_Singleton()
        ->CreateUpdateDescriptorSet(Descriptor_Manager::Graphic);
}
void RenderContext::prepare_pipeline()
{
    m_graphic_pipeline.reset(new Graphic_Pipeline);

    vert_shader.reset(new ShaderModule("D:/MoChengRT/shader/post.vert.spv"));
    frag_shader.reset(new ShaderModule("D:/MoChengRT/shader/post.frag.spv"));

    m_graphic_pipeline->Add_Shader_Modules(vert_shader->get_handle(), vk::ShaderStageFlagBits::eVertex);
    m_graphic_pipeline->Add_Shader_Modules(frag_shader->get_handle(), vk::ShaderStageFlagBits::eFragment);

    // m_graphic_pipeline->Make_Layout();
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
void RenderContext::prepare()
{
    Context_base::prepare();
    std::vector<uint32_t> indices {
        0,
        1,
        2,
        0,
        1,
        3

    };
    std::vector<float> positions {
        1.0f,
        -1.0f,
        0,

        -1.0f,
        1.0f,
        0,

        -1.0f,
        -1.0f,
        0,

        1.0f,
        1.0f,
        0

    };
    std::vector<float> uvs {
        1.0f,
        1.0f,

        0.0f,
        0.0f,

        0.0f,
        1.0f,

        1.0f,
        0.0f
    };
    index_buffer = Buffer::CreateDeviceBuffer(indices.data(), indices.size() * sizeof(indices[0]), vk::BufferUsageFlagBits::eIndexBuffer);
    vertex_buffer = Buffer::CreateDeviceBuffer(positions.data(), positions.size() * sizeof(positions[0]), vk::BufferUsageFlagBits::eVertexBuffer);
    uv_buffer = Buffer::CreateDeviceBuffer(uvs.data(), uvs.size() * sizeof(uvs[0]), vk::BufferUsageFlagBits::eVertexBuffer);
    // m_graphic_pipeline.reset(new Graphic_Pipeline);
    // // m_graphic_pipeline.
    // auto count { enable_swapchain ? 3 : 1 };

    // for (int i = 0; i < render_frame_count; i++) {

    //     // todo need add multisampler stuff
    //     // std::shared_ptr<Image> swapchain_image { new Image(
    //     //     m_swapchain->Get_Swapchain_Images()[i],
    //     //     vk::ImageLayout::eColorAttachmentOptimal,
    //     //     m_swapchain->Get_Format(),
    //     //     vk::ImageAspectFlagBits::eColor) };

    //     std::vector<std::shared_ptr<RenderTarget>>
    //         render_targets;

    //     // render_targets.emplace_back(Final_RenderTarget::Create(swapchain_image));
    //     render_targets.emplace_back(Color_RenderTarget::Create());

    //     // render_targets.emplace_back(MultiSampler_RenderTarget::Create());
    //     // render_targets.emplace_back(Depth_RenderTarget::Create());
    //     if (!m_renderpass)
    //         Prepare_RenderPass(render_targets);
    //     render_frames.emplace_back(new RenderFrame(m_renderpass, render_targets));
    // }
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

    auto& cmd = command_buffer;

    cmd->Reset();
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
    // auto pipeline = Context::Get_Singleton()->Get_Pipeline();
    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    cmd->BeginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    return cmd;
}
void RenderContext::record_command(std::shared_ptr<CommandBuffer> cmd)
{
    auto cmd_handle = cmd->get_handle();
    cmd_handle.bindIndexBuffer(index_buffer->get_handle(), 0, vk::IndexType ::eUint32);
    cmd_handle.bindVertexBuffers(0, {
                                        vertex_buffer->get_handle(),
                                        uv_buffer->get_handle(),
                                    },
                                 { 0, 0 });

    cmd_handle.bindPipeline(vk::PipelineBindPoint ::eGraphics, get_pipeline()->get_handle());
    cmd_handle.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                  get_pipeline()->get_layout(),
                                  0,
                                  Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Graphic)->get_handle(),

                                  {});
    cmd_handle.drawIndexed(6, 1, 0, 0, 0);
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
