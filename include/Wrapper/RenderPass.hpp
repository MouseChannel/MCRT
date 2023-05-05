#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class RenderPass : public Component<vk::RenderPass, RenderPass> {
public:
    RenderPass();
    ~RenderPass();
    void Add_Attachment_description(vk::AttachmentDescription attach_des);
    void Build();
    [[nodiscard("Missing subpass")]] auto& Get_Subpass()
    {
        return subpass;
    }

private:
    std::vector<vk::AttachmentDescription> attachment_descriptions;
    std::vector<vk::AttachmentReference> attach_references;
    vk::SubpassDescription subpass;
};
} // namespace MoCheng3D