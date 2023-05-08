#include "Rendering/RT_Context.hpp"
#include "Helper/math.hpp"

// #include "Helper/Ray_Tracing/RT_Manager.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Helper/math.hpp"
#include "Rendering/Context_base.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Rendering/Render_Target/RT_Color_RenderTarget.hpp"
#include "Rendering/Render_Target/Render_Target.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/RenderPass.hpp"

namespace MCRT {
RT_Context::RT_Context(std::shared_ptr<Device> device)
    : m_device(device)
{
}
RT_Context::~RT_Context()
{
}
std::vector<std::shared_ptr<RenderTarget>>& RT_Context::Get_render_targets()
{
    return render_targets;
}
std::shared_ptr<Pipeline_base> RT_Context::get_pipeline()
{
    return m_rt_pipeline;
}
std::shared_ptr<Image> RT_Context::get_out_image()
{
    return render_targets[0]->Get_Image();
}
void RT_Context::add_objs(std::vector<std::shared_ptr<Model>> objs)
{
    for (auto obj : objs) {
        AS_Builder::Get_Singleton()->add_blas_obj(obj);
    }

    AS_Builder::Get_Singleton()->build_blas();
    AS_Builder::Get_Singleton()->build_tlas();
}

// void RT_Context::Prepare_RenderPass(std::vector<std::shared_ptr<RenderTarget>>& render_targets)
// {
// }
// void RT_Context::create_renderpass()
// {
//     m_renderpass.reset(new RenderPass);
//     std::vector<vk::AttachmentDescription> all_attachments;
//     // vk::AttachmentReference colorAttachmentRef;

//     vk::AttachmentDescription color_attachment;
//     color_attachment.setFormat(vk::Format ::eR32G32B32A32Sfloat)
//         .setSamples(vk::SampleCountFlagBits ::e1)
//         .setLoadOp(vk::AttachmentLoadOp ::eClear)
//         .setStoreOp(vk::AttachmentStoreOp ::eStore)
//         .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
//         .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
//         .setInitialLayout(vk::ImageLayout ::eGeneral)
//         .setFinalLayout(vk::ImageLayout ::eGeneral);
//     m_renderpass->Add_Attachment_description(color_attachment);
//     vk::AttachmentReference color_reference;
//     color_reference.setAttachment(all_attachments.size())
//         .setLayout(vk::ImageLayout ::eColorAttachmentOptimal);
//     m_renderpass->Get_Subpass().setColorAttachments(color_reference);
//     // all_attachments.push_back(color_attachment);
//     // colorAttachmentRef  = color_reference;

//     vk::AttachmentDescription depth_attachment;

//     depth_attachment.setFormat(vk::Format ::eX8D24UnormPack32)
//         .setSamples(vk::SampleCountFlagBits ::e1)
//         .setLoadOp(vk::AttachmentLoadOp ::eClear)
//         .setStoreOp(vk::AttachmentStoreOp ::eStore)
//         .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
//         .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
//         .setInitialLayout(vk::ImageLayout ::eDepthStencilAttachmentOptimal)
//         .setFinalLayout(vk::ImageLayout ::eDepthStencilAttachmentOptimal);
//     m_renderpass->Add_Attachment_description(depth_attachment);
//     vk::AttachmentReference depth_reference;
//     depth_reference.setAttachment(all_attachments.size())
//         .setLayout(vk::ImageLayout ::eDepthAttachmentOptimal);
//     m_renderpass->Get_Subpass().setPDepthStencilAttachment(&depth_reference);
//     // all_attachments.push_back(depth_attachment);
//     // subpass
//     std::vector<vk::SubpassDescription> subpasses;
//     std::vector<vk::SubpassDependency> subpassDependencies;

//     vk::SubpassDescription subpass;
//     subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
//         .setColorAttachments(color_reference)
//         .setPDepthStencilAttachment(&depth_reference);
//     vk::SubpassDependency dependence;
//     dependence.setSrcSubpass(VK_SUBPASS_EXTERNAL)
//         .setSrcSubpass(0)
//         .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
//         .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
//         .setSrcAccessMask(vk::AccessFlagBits::eNone)
//         .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

//     m_renderpass->Build();
// }
// void RT_Context::create_framebuffer()
// {
//     std::vector<std::shared_ptr<Image>> images;
//     for (auto i : render_targets) {
//         images.push_back(i->Get_Image());
//     }
//     m_framebuffer.reset(new Framebuffer(m_renderpass, images));
//     // color_image.reset(new Image(800,
//     //                             800,
//     //                             vk::Format::eR32G32B32A32Sfloat,
//     //                             vk::ImageType::e2D,
//     //                             vk::ImageTiling::eOptimal,
//     //                             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled,
//     //                             vk::ImageAspectFlagBits::eColor,
//     //                             vk::SampleCountFlagBits::e1));
//     // depth_image.reset(new Image(800,
//     //                             800,
//     //                             vk::Format::eX8D24UnormPack32,
//     //                             vk::ImageType::e2D,
//     //                             vk::ImageTiling::eOptimal,
//     //                             vk::ImageUsageFlagBits::eDepthStencilAttachment,
//     //                             vk::ImageAspectFlagBits::eDepth,
//     //                             vk::SampleCountFlagBits::e1));
//     // m_framebuffer.reset(new Framebuffer(m_renderpass,
//     //                                     { color_image, depth_image }));
// }
void RT_Context::create_shader_bind_table()
{
    vk::PhysicalDeviceProperties2 a_;
    vk::PhysicalDeviceRayTracingPipelinePropertiesKHR b_;
    vk::StructureChain<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>
        chain { a_, b_ };
    auto device_properties = Context::Get_Singleton()
                                 ->get_device()
                                 ->Get_Physical_device()
                                 .getProperties2<vk::PhysicalDeviceProperties2,
                                                 vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    auto& rt_pipeline_properties = device_properties.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    // set up sbt data
    uint32_t missCount { 1 };
    uint32_t hitCount { 1 };
    auto handleCount = 1 + missCount + hitCount;
    uint32_t handleSize = rt_pipeline_properties.shaderGroupHandleSize;

    uint32_t handleSizeAligned = align_up(handleSize, rt_pipeline_properties.shaderGroupHandleAlignment);

    m_rgenRegion.stride = align_up(handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    m_rgenRegion.size = m_rgenRegion.stride; // The size member of pRayGenShaderBindingTable must be equal to its stride member
    m_missRegion.stride = handleSizeAligned;
    m_missRegion.size = align_up(missCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    m_hitRegion.stride = handleSizeAligned;
    m_hitRegion.size = align_up(hitCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);

    // Get the shader group handles
    uint32_t dataSize = handleCount * handleSize;
    std::vector<uint8_t> handles(dataSize);
    // auto rt_pipeline = Context::Get_Singleton()->get_rt_pipeline();
    auto res = Context::Get_Singleton()
                   ->get_device()
                   ->get_handle()
                   .getRayTracingShaderGroupHandlesKHR(m_rt_pipeline->get_handle(),
                                                       0,
                                                       handleCount,
                                                       dataSize,
                                                       handles.data());
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("failed to getRayTracingShaderGroupHandles");
    }

    // Allocate a buffer for storing the SBT.
    auto sbtSize = m_rgenRegion.size + m_missRegion.size + m_hitRegion.size + m_callRegion.size;
    m_SBT_buffer = Buffer::create_buffer(nullptr, sbtSize, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR);
    Context::Get_Singleton()->get_debugger()->set_name(m_SBT_buffer, "sbt_buffer");

    // allocate all kinds of sbt_buffer
    auto sbt_buffer_usge = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR;
    m_SBT_buffer_rgen = Buffer::create_buffer(nullptr, m_rgenRegion.size, sbt_buffer_usge);
    m_SBT_buffer_rmiss = Buffer::create_buffer(nullptr, m_missRegion.size, sbt_buffer_usge);
    m_SBT_buffer_rhit = Buffer::create_buffer(nullptr, m_hitRegion.size, sbt_buffer_usge);
    // assign all kinds of handle data
    std::vector<uint8_t> rgen_handles;
    rgen_handles.assign(handles.begin(), handles.begin() + 1 * handleSize);
    std::vector<uint8_t> rmiss_handles;
    auto rmiss_begin = handles.begin() + 1 * handleSize;
    rmiss_handles.assign(rmiss_begin, rmiss_begin + missCount * handleSize);
    std::vector<uint8_t> rhit_handles;
    auto rhit_begin = handles.begin() + (1 + missCount) * handleSize;
    rhit_handles.assign(rhit_begin, rhit_begin + hitCount * handleSize);
    //
    m_rgenRegion.setDeviceAddress(m_SBT_buffer_rgen->get_address());
    m_missRegion.setDeviceAddress(m_SBT_buffer_rmiss->get_address());
    m_hitRegion.setDeviceAddress(m_SBT_buffer_rhit->get_address());

    // TODO {missCount or hitcount} >0
    m_SBT_buffer_rgen->Update(rgen_handles.data(), handleSize);
    m_SBT_buffer_rmiss->Update(rmiss_handles.data(), handleSize);
    m_SBT_buffer_rhit->Update(rhit_handles.data(), handleSize);
}
void RT_Context::fill_render_targets()
{
    render_targets.emplace_back(RT_Color_RenderTarget::Create());
    render_targets.emplace_back(Depth_RenderTarget::Create());
}
void RT_Context::Prepare_Framebuffer()
{
    std::vector<std::shared_ptr<Image>> images;
    for (auto i : render_targets) {
        images.push_back(i->Get_Image());
    }
    m_framebuffer.reset(new Framebuffer(m_renderpass, images));
}
void RT_Context::prepare_descriptorset()
{
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(AS_Builder::Get_Singleton()->get_tlas(),
                             RT_Binding::e_tlas,
                             vk::DescriptorType::eAccelerationStructureKHR,
                             vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR,
                             Descriptor_Manager::Ray_Tracing);

    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(render_targets[0]->Get_Image(),
                             RT_Binding::e_out_image,
                             vk::DescriptorType::eStorageImage,
                             vk::ShaderStageFlagBits::eRaygenKHR,
                             Descriptor_Manager::Ray_Tracing);
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(vp_matrix->buffer,
                             Global_Binding::e_vp_matrix,
                             vp_matrix->type,
                             vp_matrix->shader_stage,
                             Descriptor_Manager::Global);

    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Ray_Tracing);

    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Global);
    Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet(Descriptor_Manager::Ray_Tracing);

    Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet(Descriptor_Manager::Global);
}
void RT_Context::prepare_pipeline()
{
    m_rt_pipeline.reset(new RT_Pipeline);
}
void RT_Context::create_uniform_buffer()
{
    glm::mat4 s {
        1.432f,
        6.764f,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1
    };

    // struct MyStruct {
    //     int a, b;
    // };
    auto ss = s[0][0];
    Mat4f a;
    a.data[0][0] = 0.1f;

    vp_matrix = UniformManager::make_uniform(V_P_Matrix { s, s }.to_shader_data(),
                                             vk::ShaderStageFlagBits::eRaygenKHR,
                                             vk::DescriptorType ::eUniformBuffer);
}

void RT_Context::prepare()
{
    auto obj = Context::Get_Singleton()->get_obj();
    add_objs({ obj });
    // Context_base::prepare();
    fill_render_targets();
    create_uniform_buffer();
    prepare_descriptorset();
    prepare_pipeline();
    create_shader_bind_table();
    m_command_buffer.reset(new CommandBuffer);
}
void RT_Context::record_command(std::shared_ptr<CommandBuffer> cmd)
{
    cmd->get_handle().traceRaysKHR(m_rgenRegion,
                                   m_missRegion,
                                   m_hitRegion,
                                   m_callRegion,
                                   800,
                                   749,
                                   1);
}
std::shared_ptr<CommandBuffer> RT_Context::BeginFrame()
{
    m_command_buffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return m_command_buffer;
}
void RT_Context::Submit()
{
    m_command_buffer->End();
    auto queue = Context::Get_Singleton()->get_device()->Get_Graphic_queue();
    vk::SubmitInfo submit_info;
    submit_info.setCommandBuffers(m_command_buffer->get_handle());
    queue.submit(submit_info);
    queue.waitIdle();
}
void RT_Context::EndFrame()
{
    m_command_buffer->End();
}
}