#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {

SwapChainTarget::SwapChainTarget()
{
    auto format = Context::Get_Singleton()->get_swapchain()->Get_Format();
   
    des.setFormat(format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp ::eClear)
        .setStoreOp(vk::AttachmentStoreOp ::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp ::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp ::eDontCare)
        .setInitialLayout(vk::ImageLayout ::eUndefined)
        .setFinalLayout(vk::ImageLayout ::ePresentSrcKHR);
    
    
    
    
}
}