#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"
namespace MoCheng3D {
Framebuffer::Framebuffer(std::vector<std::shared_ptr<Image>> images)
{

    auto render_pass = Get_Context_Singleton()->Get_RenderPass();
    auto h = Get_Context_Singleton()->Get_SwapChain()->Get_Extent2D().height;
    auto w = Get_Context_Singleton()->Get_SwapChain()->Get_Extent2D().width;

    vk::FramebufferCreateInfo create_info;
    std::vector<vk::ImageView> image_views;
    for (auto& i : images) {

        image_views.push_back(i->Get_Image_View());
    }
    create_info.setRenderPass(render_pass->Get_handle())
        .setAttachments(image_views)
        .setHeight(h)
        .setWidth(w)
        .setLayers(1);
    m_handle = Get_Context_Singleton()->Get_Device()->Get_handle().createFramebuffer(
        create_info);
}
Framebuffer::~Framebuffer()
{
    Context::Get_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroyFramebuffer(m_handle);
}
} // namespace MoCheng3D