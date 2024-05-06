#pragma once

#include "Helper/DescriptorManager.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "vulkan/vulkan.hpp"
#include <functional>
#include <memory>
namespace MCRT {

class BaseSubPass {
public:
    BaseSubPass(std::shared_ptr<DescriptorManager> descriptorManager) ;
    virtual void run_subpass();
    virtual void set_description();
    //    virtual void prepare_renderTarget();

    virtual void prepare_descriptorset( std::function<void()> prepare);
    virtual void prepare_pipeline();
    virtual void prepare_vert_shader_module(std::string vert_shader);
    virtual void prepare_frag_shader_module(std::string frag_shader);

    vk::SubpassDescription description;

protected:
    std::shared_ptr<Graphic_Pipeline> m_pipeline;
    std::vector<std::shared_ptr<ShaderModule>> shaders { 2 };
    std::shared_ptr<DescriptorManager> m_descriptorManager;
    //    std::shared_ptr<ShaderModule> vert_shader;
    //    std::shared_ptr<ShaderModule> frag_shader;
};
}
