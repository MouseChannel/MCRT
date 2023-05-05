#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class RT_Color_RenderTarget : public RenderTarget {
public:
    RT_Color_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::unique_ptr<RT_Color_RenderTarget> Create();
    void Make_Subpass(uint32_t attachment_index, vk::SubpassDescription& subpass) override;

private:
};

}