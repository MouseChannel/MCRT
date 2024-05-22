#pragma once
#include "Wrapper/BasePass.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
#include <functional>

namespace MCRT {

class ComputePass : public BasePass {
public:
    ComputePass();
    // void prepare_pipeline() override;
    void prepare_pipeline(int pc_size) override;
    void SetShaderModule(std::string shader_path);
    void SetShaderModule(std::shared_ptr<ShaderModule> shader_module);

    // void PreparePipeline();

    virtual void Dispach(vk::CommandBuffer cmd, int local_size_x, int local_size_y, int local_size_z);

    //  void Prepare_DescriptorSet(std::function<void()>)
    // void PrepareDescriptorSet(std::function<void()>);
    // void AddDescriptorSetTarget(std::shared_ptr<DescriptorSetTargetBase> target);
    int get_DescriptorSetCount() override
    {
        return 1;
    }
    auto get_pipeline(){
        return m_pipeline;
    }

protected:
    std::shared_ptr<Compute_Pipeline> m_pipeline;
    std::shared_ptr<ShaderModule> m_shader_module;
    // std::shared_ptr<DescriptorPool> m_descriptorSetPool;
    // std::shared_ptr<DescriptorSet> m_descriptorSet;
    // std::vector<std::shared_ptr<DescriptorSetTargetBase>> m_descriptorSetTarget;
};

}