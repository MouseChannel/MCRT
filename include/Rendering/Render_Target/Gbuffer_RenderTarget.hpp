
#pragma once
#include "Rendering/Render_Target/Render_Target.hpp"
namespace MCRT {

class GBuffer_RenderTarget : public RenderTarget {

public:
    GBuffer_RenderTarget(std::shared_ptr<Image> image, vk::AttachmentDescription des);
    static std::shared_ptr<GBuffer_RenderTarget> Create(vk::ImageUsageFlags usage, vk::Format format);
    void Make_Subpass(uint32_t attachment_index, std::shared_ptr<RenderPass> renderPass) override;

private:
};

}