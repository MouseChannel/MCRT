#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"

namespace MCRT {
class Final_RenderTarget : public RenderTarget {
public:
    Final_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::unique_ptr<Final_RenderTarget> Create(std::shared_ptr<Image> image);
    void Make_Subpass(uint32_t attachment_index, vk::SubpassDescription& subpass) override;

private:
};

}