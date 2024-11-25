#include "Rendering/Render_Target/Gbuffer_RenderTarget.hpp"
#include "Helper/Debugger.hpp"
#include "Wrapper/RenderPass.hpp"

namespace MCRT {
 

void GBuffer_RenderTarget::recreate(int index)
{

    auto extent = Context::Get_Singleton()->get_extent2d();

    m_image.reset(new Image(
        extent.width,
        extent.height,
        vk::Format::eR8G8B8A8Snorm,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1));
}
GBuffer_RenderTarget::GBuffer_RenderTarget()
    : RenderTarget()
{

    // vk::AttachmentDescription des;

    attachment_description.setFormat(vk::Format::eR8G8B8A8Snorm)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eColorAttachmentOptimal);

    auto extent = Context::Get_Singleton()->get_extent2d();

    m_image.reset(new Image(
        extent.width,
        extent.height,
        vk::Format::eR8G8B8A8Snorm,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1,
        1));
    // Context::Get_Singleton()->get_debugger()->set_name(m_image, "gbuffer_rendertarget");

    clear_color.color.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
} 
}
