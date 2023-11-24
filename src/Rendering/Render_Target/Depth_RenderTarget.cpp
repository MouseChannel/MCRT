#include "Rendering/Context.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
Depth_RenderTarget::Depth_RenderTarget(std::shared_ptr<Image> image,
                                       vk::AttachmentDescription des)
    : RenderTarget(image, RenderTarget::DEPTH, des)
{
    clear_color.setDepthStencil({ 1, 0 });
}
std::shared_ptr<Depth_RenderTarget> Depth_RenderTarget::Create()
{

    auto sampler_count = Context::Get_Singleton()->get_device()->Get_sampler_count();
    auto depth_format = Context::Get_Singleton()->get_device()->Get_supported_format(
        { 
            vk::Format::eD32Sfloat,
          vk::Format::eD32SfloatS8Uint,
          vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

    vk::AttachmentDescription des;

    des.setFormat(depth_format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eDepthStencilAttachmentOptimal);
    auto extent = Context::Get_Singleton()->get_extent2d();
    std::shared_ptr<Image> image { new Image(
        extent.width,
        extent.height,
        depth_format,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment  ,
        vk::ImageAspectFlagBits::eDepth,
        vk::SampleCountFlagBits::e1) };

    image->SetImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal,
                          vk::AccessFlagBits::eNone,
                          vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead,
                          vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::PipelineStageFlagBits::eEarlyFragmentTests);
    return std::shared_ptr<Depth_RenderTarget>(new Depth_RenderTarget(image, des));
}
void Depth_RenderTarget::Make_Subpass(uint32_t attachment_index,
                                      std::shared_ptr<RenderPass> renderPass)
{

    attach_reference.setAttachment(attachment_index)
        .setLayout(image->Get_image_layout());
    renderPass->Get_Subpass().setPDepthStencilAttachment(&attach_reference);
}
}