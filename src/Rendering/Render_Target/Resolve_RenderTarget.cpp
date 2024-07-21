#include "Helper/Debugger.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/Render_Target/Resover_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
// std::vector<vk::AttachmentReference> Resolve_RenderTarget::attach_references;
// Resolve_RenderTarget::Resolve_RenderTarget(std::shared_ptr<Image> image,
//                                            vk::AttachmentDescription des)
//     : RenderTarget(image, RenderTarget::COLOR, des)
// {

//     // clear_color.setColor(cur_clear_color);
//     clear_color.color.setFloat32({ 0.1f, 0.1f, 0.1f, 1.0f });
// }
void Resolve_RenderTarget::recreate(int index)
{
    auto extent = Context::Get_Singleton()->get_extent2d();
    m_image.reset(new Image(
        extent.width,
        extent.height,
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1,
        1));
    std::cout << "resolve" << m_image->Get_Image_View() << std::endl;

    Context::Get_Singleton()->get_debugger()->set_handle_name(m_image->Get_Image_View(), "new view");
}
Resolve_RenderTarget::Resolve_RenderTarget()
{
    auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();
    attachment_description.setFormat(vk::Format::eR32G32B32A32Sfloat)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eShaderReadOnlyOptimal);
    auto extent = Context::Get_Singleton()->get_extent2d();
    m_image.reset(new Image(
        extent.width,
        extent.height,
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1,
        1));
                Context::Get_Singleton()->get_debugger()->set_name(m_image, "resolve_rendertarget");
    Context::Get_Singleton()->get_debugger()->set_handle_name(m_image->Get_Image_View(), "old view");
    clear_color.color.setFloat32({ 0.1f, 0.1f, 0.1f, 1.0f });
}
// std::shared_ptr<Resolve_RenderTarget> Resolve_RenderTarget::Create()
// {

//     vk::AttachmentDescription des;
//     auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();
//     des.setFormat(vk::Format::eR32G32B32A32Sfloat)
//         .setSamples(vk::SampleCountFlagBits::e1)
//         .setLoadOp(vk::AttachmentLoadOp ::eDontCare)
//         .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
//         .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
//         .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
//         .setInitialLayout(vk::ImageLayout ::eUndefined)
//         .setFinalLayout(vk::ImageLayout ::eColorAttachmentOptimal);
//     auto extent = Context::Get_Singleton()->get_extent2d();
//     std::shared_ptr<Image> image { new Image(
//         extent.width,
//         extent.height,
//         vk::Format::eR32G32B32A32Sfloat,
//         vk::ImageType::e2D,
//         vk::ImageTiling::eOptimal,
//         vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
//         vk::ImageAspectFlagBits::eColor,
//         vk::SampleCountFlagBits::e1,
//         1) };
//     image->SetImageLayout(vk::ImageLayout::eColorAttachmentOptimal,
//                           vk::AccessFlagBits::eNone,
//                           vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead,
//                           vk::PipelineStageFlagBits::eTopOfPipe,
//                           vk::PipelineStageFlagBits::eEarlyFragmentTests);
//     return std::shared_ptr<Resolve_RenderTarget>(new Resolve_RenderTarget(image, des));
// }
// void Resolve_RenderTarget::Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass)
// {

//     attach_reference.setAttachment(attachment_index)
//         .setLayout(image->Get_image_layout());

//     //    attach_references.push_back(attach_reference);

// //    renderPass->get_attach_references().push_back(attach_reference);
//     renderPass->add_attach_reference(attach_reference);

// }
}