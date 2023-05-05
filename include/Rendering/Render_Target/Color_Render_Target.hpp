#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class Color_RenderTarget : public RenderTarget {
public:
    Color_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::unique_ptr<Color_RenderTarget> Create();
    void Make_Subpass(uint32_t attachment_index, vk::SubpassDescription& subpass) override;

private:
};

}