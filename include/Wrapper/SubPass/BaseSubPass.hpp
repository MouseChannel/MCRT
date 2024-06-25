#pragma once

// #include "Helper/DescriptorManager.hpp"
#include "Wrapper/BasePass.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "vulkan/vulkan.hpp"
#include <functional>
#include <memory>
namespace MCRT {
class DescriptorTargetBase;
class DescriptorPool;
class BaseSubPass : public BasePass {
public:
    BaseSubPass() = delete;
    BaseSubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index);

    virtual ~BaseSubPass() = default;

    virtual void prepare_vert_shader_module(std::string vert_shader) = 0;
    virtual void prepare_frag_shader_module(std::string frag_shader) = 0;
    void link_renderTarget(std::vector<int> color_renderTargets, std::vector<int> depth_renderTargets, std::vector<int> input_renderTargets, std::vector<int> resolve_renderTargets);
    void set_subpassDependency(vk::SubpassDependency dependency);
    void set_subpassDescription(vk::SubpassDescription des);
    virtual void post_prepare() = 0;
    auto& get_pipeline()
    {
        return m_pipeline;
    }
    auto& get_mesh()
    {
        return m_meshs;
    }
    virtual void recreate()
    {
    }

    vk::SubpassDescription description;
    virtual int get_DescriptorSetCount();

protected:
    std::weak_ptr<GraphicContext> m_graphicContext;
    std::shared_ptr<Graphic_Pipeline> m_pipeline;
    std::vector<std::shared_ptr<ShaderModule>> shaders { 2 };
    std::vector<std::shared_ptr<Mesh>> m_meshs;

    std::vector<vk::AttachmentReference> color_references;
    std::vector<vk::AttachmentReference> depth_references;
    std::vector<vk::AttachmentReference> input_references;
    std::vector<vk::AttachmentReference> resolve_references;

    std::vector<vk::SubpassDependency> dependencies;
    int m_subpass_index { -1 };
};
}
