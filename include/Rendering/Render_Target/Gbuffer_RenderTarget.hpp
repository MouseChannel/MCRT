
#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"
namespace MCRT {

class GBuffer_RenderTarget : public RenderTarget {

public:
    // GBuffer_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    GBuffer_RenderTarget();
    // static std::shared_ptr<GBuffer_RenderTarget> Create(vk::ImageUsageFlags usage, vk::Format format);

    // static std::shared_ptr<GBuffer_RenderTarget> Create(vk::Format format);
    // void Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass) override;
    void recreate(int index) override;
    vk::ImageLayout get_Baselayout() override
    {
        return vk::ImageLayout::eColorAttachmentOptimal;
    }
    vk::ImageLayout get_inputLayout() override
    {
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }

private:
    // vk::ClearColorValue cur_clear_color
    // {
    //     0.0f, 0.0f, 0.0f, 0.0f
    // };
};

}