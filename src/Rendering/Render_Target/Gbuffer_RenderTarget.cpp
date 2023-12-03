#include "Rendering/Render_Target/Gbuffer_RenderTarget.hpp"
#include "Wrapper/RenderPass.hpp"

namespace MCRT {

MCRT::GBuffer_RenderTarget::GBuffer_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des)
    : RenderTarget(image, RenderTarget::COLOR, des)
{
    clear_color.setColor({ 0.0f, 0.0f, 0.0f, 0.0f });
}
std::shared_ptr<GBuffer_RenderTarget> MCRT::GBuffer_RenderTarget::Create(vk::ImageUsageFlags usage, vk::Format format)
{
    vk::AttachmentDescription des;

    des.setFormat(format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eGeneral);

    auto extent = Context::Get_Singleton()->get_extent2d();
    vk::Format::eD32Sfloat;
    std::shared_ptr<Image> image { new Image(
        extent.width,
        extent.height,
        format,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eStorage | usage,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1) };

    image->SetImageLayout(vk::ImageLayout::eGeneral,
                          vk::AccessFlagBits::eNone,
                          vk::AccessFlagBits::eNone,
                          vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::PipelineStageFlagBits::eBottomOfPipe);
    return std::shared_ptr<GBuffer_RenderTarget>(new GBuffer_RenderTarget(image, des));
}
void MCRT::GBuffer_RenderTarget::Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass)
{
    attach_reference.setAttachment(attachment_index)
        .setLayout(image->Get_image_layout());

    renderPass->get_attach_references().push_back(attach_reference);
}
}
