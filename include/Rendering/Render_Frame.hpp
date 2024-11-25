#pragma once

#include "Rendering/Render_Target/Render_Target.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Device;
class Framebuffer;
class Semaphore;
class RenderPass;
class RenderFrame {

public:
    RenderFrame(std::shared_ptr<RenderPass> renderpass, std::vector<std::shared_ptr<RenderTarget>>& render_target);
    ~RenderFrame();
    [[nodiscard("Missing FrameBuffer")]] auto& Get_Framebuffer()
    {
        return m_framebuffer;
    }
    [[nodiscard("missing render_semaphore")]] auto& Get_render_semaphore()
    {
        return render_semaphore;
    }

    [[nodiscard("missing present_semaphore")]] auto& Get_present_semaphore()
    {
        return present_semaphore;
    }
    [[nodiscard("missing render_targets")]] auto& Get_render_targets()
    {
        return m_render_targets;
    }

private:
    std::vector<std::shared_ptr<RenderTarget>> m_render_targets;
    // std::shared_ptr<Device> m_device;
    std::shared_ptr<Framebuffer> m_framebuffer;
    std::shared_ptr<Semaphore> render_semaphore;

    std::shared_ptr<Semaphore> present_semaphore;
};

}