#include "Rendering/Context.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
std::vector<vk::AttachmentReference> Color_RenderTarget::attach_references;
Color_RenderTarget::Color_RenderTarget(std::shared_ptr<Image> image,
                                       vk::AttachmentDescription des)
    : RenderTarget(image, RenderTarget::COLOR, des)
{

    clear_color.setColor({ 0.1f, 0.1f, 0.1f, 1.0f });
}
std::shared_ptr<Color_RenderTarget> Color_RenderTarget::Create(std::shared_ptr<Image> swapchain_image)
{

    vk::AttachmentDescription des;
    auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();
    des.setFormat(format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::ePresentSrcKHR);
    return std::shared_ptr<Color_RenderTarget>(new Color_RenderTarget(swapchain_image, des));
}
void Color_RenderTarget::Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass)
{

    attach_reference.setAttachment(attachment_index)
        .setLayout(image->Get_image_layout());

//    attach_references.push_back(attach_reference);

    renderPass->get_attach_references().push_back(attach_reference);
}
}