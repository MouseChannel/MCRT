#include "Rendering/Render_Frame.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/Semaphore.hpp"
namespace MoCheng3D {

RenderFrame::RenderFrame(std::shared_ptr<Device> device,
    std::vector<std::shared_ptr<RenderTarget>>& render_targets)
    : m_device(device)
{
    m_render_targets = render_targets;
    std::vector<std::shared_ptr<Image>> images;
    for (auto& i : render_targets) {
        images.emplace_back(i->Get_Image());
    }
    m_framebuffer.reset(new Framebuffer(images));

    render_semaphore.reset(new Semaphore);
    present_semaphore.reset(new Semaphore);
}
RenderFrame::~RenderFrame()
{
    present_semaphore.reset();
    render_semaphore.reset();
    m_framebuffer.reset();
    m_device.reset();
    m_render_targets.clear();
}

}