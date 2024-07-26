#pragma once
#include "Rendering/ComputeContext.hpp"
#include "Wrapper/BasePass.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
#include <functional>
namespace MCRT {
template <typename T = ComputeContext>
//    requires std::derived_from<T, ComputeContext>
class ComputePass : public BasePass {
public:
    ComputePass()
    {
    }
    ComputePass(std::weak_ptr<T> computeContext):m_computeContext(computeContext)
    {
    }
    // void prepare_pipeline() override;
    
    void SetShaderModule(std::string shader_path){
        m_shader_module.reset(new ShaderModule(shader_path));
    }
    
    void SetShaderModule(std::shared_ptr<ShaderModule> shader_module){
        m_shader_module = shader_module;
    }

    // void PreparePipeline();
    void prepare_pipeline(int pc_size) override
    {
        m_pipeline.reset(new Compute_Pipeline(m_shader_module));
        m_pipeline->Make_Layout(m_descriptorSet->get_layout(), pc_size, vk::ShaderStageFlagBits::eCompute);
        m_pipeline->Build();
    }
    

    virtual void Dispach(vk::CommandBuffer cmd, int local_size_x, int local_size_y, int local_size_z){
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                               m_pipeline->get_layout(),
                               0,
                               m_descriptorSet->get_handle(),
                               {});
        cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                         m_pipeline->get_handle());
        
        cmd.dispatch(local_size_x, local_size_y, local_size_z);
    }

    //  void Prepare_DescriptorSet(std::function<void()>)
    // void PrepareDescriptorSet(std::function<void()>);
    // void AddDescriptorSetTarget(std::shared_ptr<DescriptorSetTargetBase> target);
    int get_DescriptorSetCount() override
    {
        return 1;
    }
    auto get_pipeline()
    {
        return m_pipeline;
    }

protected:
    std::weak_ptr<T> m_computeContext;
    std::shared_ptr<Compute_Pipeline> m_pipeline;
    std::shared_ptr<ShaderModule> m_shader_module;
    // std::shared_ptr<DescriptorPool> m_descriptorSetPool;
    // std::shared_ptr<DescriptorSet> m_descriptorSet;
    // std::vector<std::shared_ptr<DescriptorSetTargetBase>> m_descriptorSetTarget;
};

}
