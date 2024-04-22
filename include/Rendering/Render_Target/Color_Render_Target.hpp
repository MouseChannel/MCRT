#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class Color_RenderTarget : public RenderTarget {
public:
    Color_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::shared_ptr<Color_RenderTarget> Create(std::shared_ptr<Image> rt_out_image);
    void Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass) override;
    static std::vector<vk::AttachmentReference> attach_references;

private:
    // vk::ClearColorValue cur_clear_color
    // {
    //     0.1f, 0.1f, 0.1f, 1.0f
    // };
};

}