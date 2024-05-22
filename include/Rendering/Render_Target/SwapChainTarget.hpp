#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class SwapChainTarget : public RenderTarget {
public:
    SwapChainTarget(std::shared_ptr<Image> swapchain_img);
    // static std::shared_ptr<SwapChainTarget> Create();
    vk::ImageLayout get_Baselayout() override
    {
        return vk::ImageLayout::eColorAttachmentOptimal;
    }
    vk::ImageLayout get_inputLayout() override
    {
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }
    void recreate(int index) override;

private:
    // vk::AttachmentDescription des;
};
}