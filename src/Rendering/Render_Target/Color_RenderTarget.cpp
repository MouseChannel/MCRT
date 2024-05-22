#include "Rendering/Context.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"

#include "Wrapper/Image.hpp"

#include "Helper/Debugger.hpp"
#include "Wrapper/SwapChain.hpp"
namespace MCRT {

Color_RenderTarget::Color_RenderTarget()
    : RenderTarget()
{
    attachment_description.setFormat(vk::Format::eR32G32B32A32Sfloat)
        .setSamples(vk::SampleCountFlagBits::e8)
        .setLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eColorAttachmentOptimal);
    auto extent = Context::Get_Singleton()->get_extent2d();

    m_image.reset(new Image(
        extent.width,
        extent.height,
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e8));
    Context::Get_Singleton()->get_debugger()->set_name(m_image, "color_rendertarget");
    clear_color.color.setFloat32({ 0.1f, 0.1f, 0.1f, 1.0f });
}
// std::shared_ptr<Color_RenderTarget> Color_RenderTarget::Create(std::shared_ptr<Image> swapchain_image)
// {

//     vk::AttachmentDescription des;
//     auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();
//     des.setFormat(format)
//         .setSamples(vk::SampleCountFlagBits::e1)
//         .setLoadOp(vk::AttachmentLoadOp ::eClear)
//         .setStoreOp(vk::AttachmentStoreOp ::eStore)
//         .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
//         .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
//         .setInitialLayout(vk::ImageLayout ::eUndefined)
//         .setFinalLayout(vk::ImageLayout ::ePresentSrcKHR);
//     return std::shared_ptr<Color_RenderTarget>(new Color_RenderTarget(swapchain_image, des));
// }

void Color_RenderTarget::recreate(int index)
{
    // vk::AttachmentDescription des;

    // des.setFormat(vk::Format::eR32G32B32A32Sfloat)
    //     .setSamples(vk::SampleCountFlagBits::e8)
    //     .setLoadOp(vk::AttachmentLoadOp ::eDontCare)
    //     .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
    //     .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
    //     .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
    //     .setInitialLayout(vk::ImageLayout ::eUndefined)
    //     .setFinalLayout(vk::ImageLayout ::eColorAttachmentOptimal);
    auto extent = Context::Get_Singleton()->get_extent2d();

    m_image.reset(new Image(
        extent.width,
        extent.height,
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e8));
    // image->SetImageLayout(vk::ImageLayout::eColorAttachmentOptimal,
    //                       vk::AccessFlagBits::eNone,
    //                       vk::AccessFlagBits::eNone,
    //                       vk::PipelineStageFlagBits::eTopOfPipe,
    //                       vk::PipelineStageFlagBits::eBottomOfPipe);
}
// std::shared_ptr<Color_RenderTarget> Color_RenderTarget::Create()
// {

//     vk::AttachmentDescription des;

//     des.setFormat(vk::Format::eR32G32B32A32Sfloat)
//         .setSamples(vk::SampleCountFlagBits::e8)
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
//         vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
//         vk::ImageAspectFlagBits::eColor,
//         vk::SampleCountFlagBits::e8) };
//     // image->SetImageLayout(vk::ImageLayout::eColorAttachmentOptimal,
//     //                       vk::AccessFlagBits::eNone,
//     //                       vk::AccessFlagBits::eNone,
//     //                       vk::PipelineStageFlagBits::eTopOfPipe,
//     //                       vk::PipelineStageFlagBits::eBottomOfPipe);
//     return std::shared_ptr<Color_RenderTarget>(new Color_RenderTarget(image, des));
// }
// void Color_RenderTarget::Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass)
// {

//     attach_reference.setAttachment(attachment_index)
//         .setLayout(image->Get_image_layout());

//     //    attach_references.push_back(attach_reference);

// //    renderPass->get_attach_references().push_back(attach_reference);
//     renderPass->add_attach_reference(attach_reference);

// }
}