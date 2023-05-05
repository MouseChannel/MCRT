#include "Rendering/Context.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
Color_RenderTarget::Color_RenderTarget(std::shared_ptr<Image> image,
                                       vk::AttachmentDescription des)
    : RenderTarget(image, des)
{
    clear_color.setColor({ 0.1f, 0.1f, 0.1f, 1.0f });
}
std::unique_ptr<Color_RenderTarget> Color_RenderTarget::Create()
{

    std::shared_ptr<Image> color_image { new Image(800,
                                                   800,
                                                   vk::Format::eR32G32B32A32Sfloat,
                                                   vk::ImageType::e2D,
                                                   vk::ImageTiling::eOptimal,
                                                   vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled,
                                                   vk::ImageAspectFlagBits::eColor,
                                                   vk::SampleCountFlagBits::e1) };
    color_image->SetImageLayout(vk::ImageLayout::eGeneral, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe);
    vk::AttachmentDescription des;
    // auto la = Context::Get_Singleton()->Get_SwapChain()->Get_handle().

    des.setFormat(vk::Format::eR32G32B32A32Sfloat)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::ePresentSrcKHR);
    return std::unique_ptr<Color_RenderTarget>(new Color_RenderTarget(color_image, des));
}
void Color_RenderTarget::Make_Subpass(uint32_t attachment_index, vk::SubpassDescription& subpass)
{

    attach_reference.setAttachment(attachment_index)
        .setLayout(image->Get_image_layout());
    // subpass.setColorAttachments(attach_reference);
    subpass.setColorAttachments(attach_reference);
}
}