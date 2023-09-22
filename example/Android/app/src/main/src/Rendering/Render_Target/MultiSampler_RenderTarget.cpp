#include "Rendering/Context.hpp"
#include "Rendering/Render_Target/MultiSampler_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MoCheng3D {
MultiSampler_RenderTarget::MultiSampler_RenderTarget(
    std::shared_ptr<Image> image, vk::AttachmentDescription des)
    : RenderTarget(image, des)
{

    clear_color.setColor({ 0.1f, 0.1f, 0.1f, 1.0f });
}
std::unique_ptr<MultiSampler_RenderTarget> MultiSampler_RenderTarget::Create()
{

    auto sampler_count = Context::Get_Singleton()->Get_Device()->Get_sampler_count();
    auto swapchain_format = Context::Get_Singleton()->Get_SwapChain()->Get_Format();
    auto extent = Context::Get_Singleton()->Get_SwapChain()->Get_Extent2D();
    std::shared_ptr<Image> image {
        new Image(extent.width, extent.height, swapchain_format, vk::ImageType::e2D,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,
            sampler_count)
    };
    image->SetImageLayout(vk::ImageLayout::eColorAttachmentOptimal,
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eColorAttachmentWrite,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eColorAttachmentOutput);

    vk::AttachmentDescription des;

    des.setFormat(swapchain_format)
        .setSamples(sampler_count)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eColorAttachmentOptimal);
    return std::unique_ptr<MultiSampler_RenderTarget>(new MultiSampler_RenderTarget(image, des));
}
void MultiSampler_RenderTarget::Make_Subpass(uint32_t attachment_index,
    vk::SubpassDescription& subpass)
{
    attach_reference.setAttachment(attachment_index)
        .setLayout(image->Get_image_layout());
    subpass.setColorAttachments(attach_reference);
}
}