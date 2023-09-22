#include "Rendering/Render_Target/Final_RenderTarget.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MoCheng3D {
Final_RenderTarget::Final_RenderTarget(std::shared_ptr<Image> image,
    vk::AttachmentDescription des)
    : RenderTarget(image, des)
{
    clear_color.setColor({ 0.1f, 0.1f, 0.1f, 1.0f });
}
std::unique_ptr<Final_RenderTarget> Final_RenderTarget::Create(std::shared_ptr<Image> image)
{

    auto swapchain_format = Context::Get_Singleton()->Get_SwapChain()->Get_Format();

    vk::AttachmentDescription des;
    // auto la = Context::Get_Singleton()->Get_SwapChain()->Get_handle().

    des.setFormat(swapchain_format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::ePresentSrcKHR);
    return std::unique_ptr<Final_RenderTarget>(new Final_RenderTarget(image, des));
}
void Final_RenderTarget::Make_Subpass(uint32_t attachment_index,
    vk::SubpassDescription& subpass)
{

    attach_reference.setAttachment(attachment_index)
        .setLayout(image->Get_image_layout());
    // subpass.setColorAttachments(attach_reference);
    subpass.setResolveAttachments(attach_reference);
}
}