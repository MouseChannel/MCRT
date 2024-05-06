#include "Rendering/Render_Target/DepthTarget.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"

namespace MCRT {

DepthTarget::DepthTarget()
{

    auto sampler_count = Context::Get_Singleton()->get_device()->Get_sampler_count();
    auto depth_format = Context::Get_Singleton()->get_device()->Get_supported_format(
        { vk::Format::eD32Sfloat,
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
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::ImageAspectFlagBits::eDepth,
        vk::SampleCountFlagBits::e1) };

    image->SetImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal,
                          vk::AccessFlagBits::eNone,
                          vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead,
                          vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::PipelineStageFlagBits::eEarlyFragmentTests);
}
}
