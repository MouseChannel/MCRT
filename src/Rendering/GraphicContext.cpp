#include "Rendering/GraphicContext.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/ComputePass.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Rendering/Render_Frame.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Rendering/Render_Target/Gbuffer_RenderTarget.hpp"

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
        // m_swapchain.reset(new SwapChain);
        // Context::Get_Singleton()->set_swapchain(m_swapchain);
        Context::Get_Singleton()->get_swapchain().reset(new SwapChain);

        m_swapchain = Context::Get_Singleton()->get_swapchain();
        render_frame_count = m_swapchain->Get_Swapchain_Image_size();
    }
    for (int i = 0; i < render_frame_count; i++) {
        fences.emplace_back(new Fence);
    }
}

GraphicContext::GraphicContext()
{
    command_buffer.reset();
    fences.clear();
    m_renderpass.reset();
    render_frames.clear();
    m_swapchain.reset();
    m_device.reset();
}

std::shared_ptr<Pipeline_base> GraphicContext::get_pipeline()
{
    throw std::runtime_error("graphic context has no global Pipeline, only has subpass_Pipeline! ");
    //    return m_graphic_pipeline;
}

// std::shared_ptr<Pipeline_base> GraphicPass::get_pipeline2()
//{
//     return m_skybox_pipeline;
// }
void GraphicContext::fill_render_targets()
{
    auto count { enable_swapchain ? render_frame_count : 1 };

    all_rendertargets.clear();
    all_rendertargets.resize(count);
    { // recreate renderTarget
        for (int i = 0; i < all_rendertargets.size(); i++) {
            auto cur_swapchain_img = m_swapchain->Get_Images()[i];
            for (int j = 0; j < all_rendertargets[i].size(); j++) {
                auto& cur = all_rendertargets[i][j];
                switch (cur->type) {
                case RenderTarget::COLOR:
                    all_rendertargets[i][j] = Color_RenderTarget::Create(cur_swapchain_img);
                    break;
                case RenderTarget::DEPTH:
                    all_rendertargets[i][j] = Depth_RenderTarget::Create();
                    break;
                case RenderTarget::GBUFFER:
                    all_rendertargets[i][j] = GBuffer_RenderTarget::Create(cur->Get_Image()->Get_image_format());
                    break;
                }
            }
            // std::for_each(all_rendertargets[i].begin(), all_rendertargets[i].end(), [&](auto& old_item) {
            // });
        }
    }
    //     for (auto i { 0 }; i < count; i++) {
    //         //        auto swapchain_image_handle = m_swapchain->Get_Swapchain_ImagesHandle()[i];
    //         //        std::shared_ptr<Image> swapchain_image {
    //         //            new Image(swapchain_image_handle,
    //         //                      vk::ImageLayout::eColorAttachmentOptimal,
    //         //                      m_swapchain->Get_Format(),
    //         //                      vk::ImageAspectFlagBits::eColor)
    //         //        };

    //         auto swapchain_image = m_swapchain->Get_Images()[i];
    //         all_rendertargets[i].emplace_back(Color_RenderTarget::Create(swapchain_image));
    //         all_rendertargets[i].emplace_back(Depth_RenderTarget::Create());

    // #ifndef VK_USE_PLATFORM_ANDROID_KHR

    //         all_rendertargets[i].emplace_back(GBuffer_RenderTarget::Create(vk::ImageUsageFlagBits::eColorAttachment, vk::Format::eR32G32B32A32Sfloat));
    //         all_rendertargets[i].emplace_back(GBuffer_RenderTarget::Create(vk::ImageUsageFlagBits::eColorAttachment, vk::Format::eR32G32B32A32Sfloat));
    //         // depth
    //         all_rendertargets[i].emplace_back(GBuffer_RenderTarget::Create(vk::ImageUsageFlagBits::eColorAttachment, vk::Format::eR32Sfloat));
    //         all_rendertargets[i].emplace_back(GBuffer_RenderTarget::Create(vk::ImageUsageFlagBits::eColorAttachment, vk::Format::eR32G32B32A32Sfloat));
    // #endif
    //     }
}
// void GraphicContext::AddRenderTarget(std::shared_ptr<RenderTarget> renderTarget)
// {
//     for (int i = 0; i < all_rendertargets.size(); i++) {
//         all_rendertargets[i].push_back(renderTarget);
//     }
// }
int GraphicContext::AddSwapchainRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        auto swapchain_image = m_swapchain->Get_Images()[i];
        // AddRenderTarget(Color_RenderTarget::Create(swapchain_image));
        all_rendertargets[i].push_back(Color_RenderTarget::Create(swapchain_image));
    }
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddDepthRenderTarget()
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        auto swapchain_image = m_swapchain->Get_Images()[i];
        all_rendertargets[i].push_back(Depth_RenderTarget::Create());
    }
    return all_rendertargets[0].size() - 1;
}
int GraphicContext::AddGbufferRenderTarget(vk::Format format)
{
    for (int i = 0; i < all_rendertargets.size(); i++) {
        auto swapchain_image = m_swapchain->Get_Images()[i];
        all_rendertargets[i].push_back(GBuffer_RenderTarget::Create(format));
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

void GraphicContext::prepare_descriptorset(std::function<void()> prepare)
{
    //    for (int i = 0; i < m_subpasses.size(); ++i) {
    //        m_subpasses[i]->prepare_descriptorset(prepare);
    //    }
    prepare();
    get_descriptor_manager()->CreateDescriptorPool(DescriptorManager::Graphic);
    get_descriptor_manager()
        ->update_descriptor_set(DescriptorManager::Graphic);
    //    Descriptor_Manager::Get_Singleton()
    //        ->CreateDescriptorPool(Descriptor_Manager::Graphic);
    //    Descriptor_Manager::Get_Singleton()
    //        ->update_descriptor_set(Descriptor_Manager::Graphic);
}
 
void GraphicContext::prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size)
{

    throw std::runtime_error("graphic context has no global Pipeline, only has subpass_Pipeline! ");
    //    {
    //        // main
    //        m_graphic_pipeline.reset(new Graphic_Pipeline(
    //            { shader_modules[Graphic_Pipeline::Main_VERT],
    //              shader_modules[Graphic_Pipeline::Main_FRAG] },
    //            sets,
    //            push_constants_size));
    //
    //        auto binds = Vertex::make_bind();
    //        auto attrs = Vertex::make_attr();
    //
    //        m_graphic_pipeline->Make_VertexInput(binds, attrs);
    //        m_graphic_pipeline->Make_VertexAssembly();
    //        m_graphic_pipeline->Make_viewPort();
    //        m_graphic_pipeline->Make_MultiSample();
    //        m_graphic_pipeline->Make_Resterization();
    //        m_graphic_pipeline->Make_attach();
    //        m_graphic_pipeline->Make_Blend();
    //        m_graphic_pipeline->Make_DepthTest();
    //
    //        // m_graphic_pipeline->Build_Pipeline(Context::Get_Singleton()->get_graphic_context()->Get_render_pass());
    //    }
    //    {
    //        // skybox
    //        if (!shader_modules[Graphic_Pipeline::Skybox_VERT]) {
    //            return;
    //        }
    //        m_skybox_pipeline.reset(new Graphic_Pipeline(
    //            { shader_modules[Graphic_Pipeline::Skybox_VERT],
    //              shader_modules[Graphic_Pipeline::Skybox_FRAG] },
    //            sets,
    //            push_constants_size));
    //
    //        auto binds = Vertex::make_bind();
    //        auto attrs = Vertex::make_attr();
    //
    //        m_skybox_pipeline->Make_VertexInput(binds, attrs);
    //        m_skybox_pipeline->Make_VertexAssembly();
    //        m_skybox_pipeline->Make_viewPort();
    //        m_skybox_pipeline->Make_MultiSample();
    //        m_skybox_pipeline->Make_Resterization(vk::CullModeFlagBits::eNone);
    //        m_skybox_pipeline->Make_attach();
    //        m_skybox_pipeline->Make_Blend();
    //        m_skybox_pipeline->Make_DepthTest(false);
    //
    //        // m_skybox_pipeline->Build_Pipeline(Context::Get_Singleton()->get_graphic_context()->Get_render_pass());
    //    }
}
 
void GraphicContext::prepare()
{
    fill_render_targets();
    // AddSwapchainRenderTarget();
    // AddDepthRenderTarget();
    // Prepare_RenderPass();
    // Prepare_Framebuffer();

    // command_buffer.reset(new CommandBuffer);
}

void GraphicContext::post_prepare()
{
    Prepare_RenderPass();
    Prepare_Framebuffer();
    for (auto& subpass : subpasses) {
        subpass->post_prepare();
    }
    //    m_graphic_pipeline->Build_Pipeline(Context::Get_Singleton()->get_graphic_context()->Get_render_pass());
    //    m_skybox_pipeline->Build_Pipeline(Context::Get_Singleton()->get_graphic_context()->Get_render_pass());

    // command_buffer = Context::Get_Singleton()->get_compute_context()->get_commandbuffer();
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
void GraphicContext::record_command(std::shared_ptr<CommandBuffer> cmd)
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
    auto extent2d = Context::Get_Singleton()->get_extent2d();

    cmd->get_handle().setViewport(0,
                                  vk::Viewport()
                                      .setHeight(extent2d.height)
                                      .setWidth(extent2d.width)
                                      .setMinDepth(0)
                                      .setMaxDepth(1)
                                      .setX(0)
                                      .setY(0));
    cmd->get_handle().setScissor(0,
                                 vk::Rect2D()
                                     .setExtent(extent2d)
                                     .setOffset(vk::Offset2D()
                                                    .setX(0)
                                                    .setY(0)));

    Context::Get_Singleton()->get_debugger()->set_name(cmd, "render command_buffer");
#endif
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
    try {

        //        VkSubmitInfo sub {};
        //        sub.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //        sub.commandBufferCount = 1;
        //        sub.pCommandBuffers = (VkCommandBuffer*)&command_buffer->get_handle();
        //        sub.pWaitSemaphores = (VkSemaphore*)&Get_cur_render_semaphore()->get_handle();
        //        sub.waitSemaphoreCount = 1;
        //        sub.pWaitDstStageMask = (VkPipelineStageFlags*)&wait_mask;
        //        sub.signalSemaphoreCount = 1;
        //        sub.pSignalSemaphores = (VkSemaphore*)&Get_cur_present_semaphore()->get_handle();

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        auto fence = Get_cur_fence()->get_handle();
#else
        auto fence = Get_cur_fence()->get_handle();
#endif
        graphic_queue.submit(submit_info, fence);
        //        auto res =  vkQueueSubmit((VkQueue)graphic_queue,
        //                                 1,
        //                                 &sub,
        //                                  fence);

        // auto res = graphic_queue.submit(1, &submit_info, Get_cur_fence()->get_handle(), m_device->get_handle());
        //        if (res == VK_ERROR_DEVICE_LOST) {
        //            auto r = m_device->get_handle().getFaultInfoEXT();
        //            int tt = 0;
        //        }
    } catch (std::exception e) {
        // auto r = m_device->get_handle().getFaultInfoEXT();
        int rr = 9;
    }
    //    graphic_queue.waitIdle();
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
        std::cout << "present fail" << std::endl;
        re_create();
    }
    //    re_create();

    if (present_result != vk::Result::eSuccess) {
        int r = 0;
    }
    // current_frame++;
    // current_frame %= render_frame_count;
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
    std::cout << cur_width << cur_height << std::endl;
    Context::Get_Singleton()->get_swapchain().reset(new SwapChain);
    m_swapchain = Context::Get_Singleton()->get_swapchain();
    Context::Get_Singleton()->set_extent2d(cur_width, cur_height);
    fill_render_targets();
    Prepare_Framebuffer();
    Context::Get_Singleton()
        ->get_camera()
        ->setPerpective(90, (float)cur_width / (float)cur_height, 0.1f, 10000);
    //---
    Context::Get_Singleton()->re_create_context();
#endif
}

void GraphicContext::re_create_swapchain()
{
}
void GraphicContext::AddSubPass(std::shared_ptr<BaseSubPass> subpass)
{
    // subpass->subpass_index = m_subpasses.size();
    subpasses.push_back(subpass);
}
void GraphicContext::AddSubPassDependency(vk::SubpassDependency dependency)
{
    m_subpass_dependencies.push_back(dependency);
}
} // namespace MoCheng3D
