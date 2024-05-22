#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class Resolve_RenderTarget : public RenderTarget {
public:
    Resolve_RenderTarget();
    // Resolve_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    // static std::shared_ptr<Resolve_RenderTarget> Create();

    // void Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass) override;
    // static std::vector<vk::AttachmentReference> attach_references;
    void recreate(int index) override;
    vk::ImageLayout get_Baselayout() override
    {
        return vk::ImageLayout::eGeneral;
    }
    vk::ImageLayout get_inputLayout() override
    {
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }

private:
    // vk::ClearColorValue cur_clear_color
    // {
    //     0.1f, 0.1f, 0.1f, 1.0f
    // };
};

}