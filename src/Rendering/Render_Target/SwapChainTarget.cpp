#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "Context/Context.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {

SwapChainTarget::SwapChainTarget(std::shared_ptr<Image> swapchain_img)
{
    auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();

    attachment_description.setFormat(format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::ePresentSrcKHR);
    m_image = swapchain_img;
    clear_color.color.setFloat32({ 0.1f, 0.1f, 0.1f, 1.0f });
}

void SwapChainTarget::recreate(int index)
{
    m_image = Context::Get_Singleton()->get_swapchain()->Get_Images()[index];
}
}