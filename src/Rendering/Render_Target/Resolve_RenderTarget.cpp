#include "Context/Context.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/Render_Target/Resover_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
 
void Resolve_RenderTarget::recreate(int index)
{
    auto extent = Context::Get_Singleton()->get_extent2d();
    m_image.reset(new Image(
        extent.width,
        extent.height,
        // vk::Format::eR32G32B32A32Sfloat,
        vk::Format::eR8G8B8A8Snorm,

        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eInputAttachment,
        vk::ImageAspectFlagBits::eColor,
        vk::SampleCountFlagBits::e1,
        1));
     
    Context::Get_Singleton()->get_debugger()->set_handle_name(m_image->Get_Image_View(), "new view");
}
Resolve_RenderTarget::Resolve_RenderTarget()
{
    auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();
        attachment_description.setFormat(vk::Format::eR8G8B8A8Snorm)

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
        vk::Format::eR8G8B8A8Snorm,
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
 
}