#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include "Wrapper/SubPass/BaseSubPass.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class RenderPass : public Component<vk::RenderPass, RenderPass> {
public:
    RenderPass();
    ~RenderPass();
    void Add_Attachment_description(vk::AttachmentDescription attach_des);
    void Build();
    void Build(std::vector<std::shared_ptr<BaseSubPass>> subpasses, std::vector<vk::SubpassDependency> subpass_dependency);
    [[nodiscard("Missing subpass")]] auto& Get_Subpass()
    {
        return subpass;
    }
    auto& get_attach_references()
    {
        return attach_references;
    }
    void add_attach_reference(vk::AttachmentReference attachmentReference)
    {
        attach_references.push_back(attachmentReference);
    }

private:
    std::vector<vk::AttachmentDescription> attachment_descriptions;
    std::vector<vk::AttachmentReference> attach_references;
    vk::SubpassDescription subpass;
};
} // namespace MoCheng3D