#pragma once
#include "Wrapper/Image.hpp"
#include "vulkan/vulkan.hpp"

namespace MCRT {
class RenderTarget {
public:
    RenderTarget(std::shared_ptr<Image>, vk::AttachmentDescription);
    ~RenderTarget();
    [[nodiscard("Missing format")]] auto& Get_Extent()
    {
        return extent;
    }

    [[nodiscard("Missing Image")]] auto& Get_Image()
    {
        return image;
    }
    [[nodiscard("missing attach_des")]] auto& Get_attachment_description()
    {
        return attachment_description;
    }
    virtual void Make_Subpass(uint32_t attachment_index, vk::SubpassDescription& subpass) = 0;

    [[nodiscard("missing render_target")]] auto& Get_clearcolor()
    {
        return clear_color;
    }

protected:
    vk::Extent2D extent;

    std::shared_ptr<Image> image;

    vk::AttachmentDescription attachment_description;
    vk::AttachmentReference attach_reference;
    vk::ClearValue clear_color;
};

}