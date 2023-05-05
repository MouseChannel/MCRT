#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class MultiSampler_RenderTarget : public RenderTarget {

public:
    MultiSampler_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::unique_ptr<MultiSampler_RenderTarget> Create();
    void Make_Subpass(uint32_t attachment_index, vk::SubpassDescription& subpass) override;

private:
};

}