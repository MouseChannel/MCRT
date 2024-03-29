#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>
namespace MCRT {
RenderPass::RenderPass()
{
}

void RenderPass::Add_Attachment_description(vk::AttachmentDescription attach_des)
{
    attachment_descriptions.push_back(attach_des);
}
void RenderPass::Build()
{
    subpass.setColorAttachments(attach_references);
    

    vk::SubpassDependency subpass_dependency {};

    // vk::VK_SUBPASS_EXTERNAL
    subpass_dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo create_info;
    create_info.setAttachments(attachment_descriptions);
    create_info.setSubpasses(subpass);

    create_info.setDependencies(subpass_dependency);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createRenderPass(create_info);
}
RenderPass::~RenderPass()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroyRenderPass(m_handle);
}
} // namespace MoCheng3D