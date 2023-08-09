#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
Framebuffer::Framebuffer(std::shared_ptr<RenderPass> renderpass, std::vector<std::shared_ptr<Image>> images)
{

    //   render_pass = Get_Context_Singleton()->get_renderpass();
    auto h = Get_Context_Singleton()->get_extent2d().height;
    auto w = Get_Context_Singleton()->get_extent2d().width;

    std::vector<vk::ImageView> image_views;
    for (auto& i : images) {

        image_views.push_back(i->Get_Image_View());
    }
    vk::FramebufferCreateInfo create_info;
    create_info.setRenderPass(renderpass->get_handle())
        .setAttachments(image_views)
        .setHeight(h)
        .setWidth(w)
        .setLayers(1);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createFramebuffer(create_info);
}
Framebuffer::~Framebuffer()
{
    Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .destroyFramebuffer(m_handle);
}
} // namespace MoCheng3D