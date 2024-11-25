#include "Context/Context.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"

#include "Wrapper/Image.hpp"

#include "Helper/Debugger.hpp"
#include "Wrapper/SwapChain.hpp"
namespace MCRT {

Color_RenderTarget::Color_RenderTarget()
    : RenderTarget()
{
    // attachment_description.setFormat(vk::Format::eR32G32B32A32Sfloat)
    attachment_description.setFormat(vk::Format::eR8G8B8A8Snorm)
        // .setSamples(vk::SampleCountFlagBits::e8)
        .setSamples(vk::SampleCountFlagBits::e1)

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
        // vk::Format::eR32G32B32A32Sfloat,
        vk::Format::eR8G8B8A8Snorm,

        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        // vk::SampleCountFlagBits::e8));
        vk::SampleCountFlagBits::e1));

    Context::Get_Singleton()->get_debugger()->set_name(m_image, "color_rendertarget");
    clear_color.color.setFloat32({ 0.1f, 0.1f, 0.1f, 1.0f });
}
 
void Color_RenderTarget::recreate(int index)
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
}