#pragma once

// #include "Helper/DescriptorManager.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "vulkan/vulkan.hpp"
#include <functional>
#include <memory>
namespace MCRT {

class BaseSubPass {
public:
    BaseSubPass() = delete;
    BaseSubPass(std::weak_ptr<GraphicContext> graphicContext);
    virtual ~BaseSubPass() = default;
    //    virtual void run_subpass();
    //    virtual void set_description();
    //    virtual void prepare_renderTarget();

    //    virtual void prepare_descriptorset(std::function<void()> prepare);
    virtual void prepare_pipeline(int pc_size) = 0;
    // virtual void draw(vk::CommandBuffer& cmd, std::vector<std::shared_ptr<Mesh>>& meshs, void* push_constant) = 0;
    // virtual void draw1(vk::CommandBuffer& cmd) = 0;

    virtual void prepare_vert_shader_module(std::string vert_shader) = 0;
    virtual void prepare_frag_shader_module(std::string frag_shader) = 0;
    void link_renderTarget(std::vector<int> color_renderTargets, std::vector<int> depth_renderTargets, std::vector<int> input_renderTargets);
    void set_subpassDependency(vk::SubpassDependency dependency);
    void set_subpassDescription(vk::SubpassDescription des);
    virtual void post_prepare() = 0;
    auto& get_pipeline()
    {
        return m_pipeline;
    }
    vk::SubpassDescription description;
    //    int get_index(){
    //        return index;
    //    }
    // int subpass_index = -1;

protected:
    std::weak_ptr<GraphicContext> m_graphicContext;
    std::shared_ptr<Graphic_Pipeline> m_pipeline;
    std::vector<std::shared_ptr<ShaderModule>> shaders { 2 };
    //    std::shared_ptr<DescriptorManager> m_descriptorManager;
    //    std::shared_ptr<ShaderModule> vert_shader;
    //    std::shared_ptr<ShaderModule> frag_shader;
    //    std::vector<vk::AttachmentReference> references;
    std::vector<vk::AttachmentReference> color_references;
    std::vector<vk::AttachmentReference> depth_references;
    std::vector<vk::AttachmentReference> input_references;

    std::vector<vk::SubpassDependency> dependencies;
};
}
