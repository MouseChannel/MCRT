#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class Color_RenderTarget : public RenderTarget {
public:
    // Color_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    Color_RenderTarget();
    // static std::shared_ptr<Color_RenderTarget> Create(std::shared_ptr<Image> rt_out_image);

    // static std::shared_ptr<Color_RenderTarget> Create();
    void recreate(int index) override;

    // void Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass) override;
    // static std::vector<vk::AttachmentReference> attach_references;
    vk::ImageLayout get_Baselayout() override
    {
        return vk::ImageLayout::eColorAttachmentOptimal;
    }
    vk::ImageLayout get_inputLayout() override
    {
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }

private:
};

}