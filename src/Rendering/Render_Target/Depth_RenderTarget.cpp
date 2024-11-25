#include "Context/Context.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
 
void Depth_RenderTarget::recreate(int index)
{
    auto depth_format = Context::Get_Singleton()->get_device()->Get_supported_format(
        { vk::Format::eD32Sfloat,
          vk::Format::eD32SfloatS8Uint,
          vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    auto extent = Context::Get_Singleton()->get_extent2d();
    m_image.reset(new Image(
        extent.width,
        extent.height,
        depth_format,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::ImageAspectFlagBits::eDepth,
        vk::SampleCountFlagBits::e1));
}
Depth_RenderTarget::Depth_RenderTarget()
{
    auto depth_format = Context::Get_Singleton()->get_device()->Get_supported_format(
        { vk::Format::eD32Sfloat,
          vk::Format::eD32SfloatS8Uint,
          vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);

    attachment_description.setFormat(depth_format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::eDepthStencilAttachmentOptimal);
    auto extent = Context::Get_Singleton()->get_extent2d();
    m_image.reset(new Image(
        extent.width,
        extent.height,
        depth_format,
        vk::ImageType::e2D,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::ImageAspectFlagBits::eDepth,
        vk::SampleCountFlagBits::e1));
    clear_color.depthStencil.setDepth(1.f);
}
 
}