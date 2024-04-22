#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class Depth_RenderTarget : public RenderTarget {

public:
    Depth_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::shared_ptr<Depth_RenderTarget> Create();
    void Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass) override;

private:
    // vk::ClearDepthStencilValue cur_clear_color {
    //     1, 0
    // };
};

}