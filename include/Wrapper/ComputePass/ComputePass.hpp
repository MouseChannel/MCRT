#pragma once
#include "Rendering/ComputeContext.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
namespace MCRT {
template <typename T = ComputeContext>
//    requires std::derived_from<T, ComputeContext>
class ComputePass {
public:
    ComputePass( std::initializer_list<std::shared_ptr<DescriptorSet>> sets, int pc_size, std::string shader_path)
    {
        m_sets = sets;

        SetShaderModule(shader_path);
        prepare_pipeline(pc_size);
    }
    ComputePass(std::initializer_list<std::shared_ptr<DescriptorSet>> sets, int pc_size, std::shared_ptr<ShaderModule> shader_module)
    {
        m_sets = sets;

        SetShaderModule(shader_module);
        prepare_pipeline(pc_size);
    }
    // void prepare_pipeline() override;

    void SetShaderModule(std::string shader_path)
    {
        m_shader_module.reset(new ShaderModule(shader_path));
    }

    void SetShaderModule(std::shared_ptr<ShaderModule> shader_module)
    {
        m_shader_module = shader_module;
    }

    // void PreparePipeline();
    void prepare_pipeline(int pc_size)
    {
        m_pipeline.reset(new Compute_Pipeline(m_shader_module));
        std::vector<vk::DescriptorSetLayout> layouts;
        for (auto i : m_sets)
            layouts.push_back(i->get_layout());
        m_pipeline->Make_Layout(layouts, pc_size, vk::ShaderStageFlagBits::eCompute);
        m_pipeline->Build();
    }

    virtual void Dispach(vk::CommandBuffer cmd, int local_size_x, int local_size_y, int local_size_z, int descriptor_index = 0)
    {
        std::vector<vk::DescriptorSet> native_sets(m_sets.size());
        std::transform(m_sets.begin(), m_sets.end(), native_sets.begin(), [descriptor_index](std::shared_ptr<DescriptorSet> set) {
            return set->get_handle()[descriptor_index];
        });
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                               m_pipeline->get_layout(),
                               0,
                               native_sets,
                               {});
        cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                         m_pipeline->get_handle());

        cmd.dispatch(local_size_x, local_size_y, local_size_z);
    }

    //  void Prepare_DescriptorSet(std::function<void()>)
    // void PrepareDescriptorSet(std::function<void()>);
    // void AddDescriptorSetTarget(std::shared_ptr<DescriptorSetTargetBase> target);
    // int get_DescriptorSetCount() override
    // {
    //     return 1;
    // }
    auto get_pipeline()
    {
        return m_pipeline;
    }

protected:
    T* m_computeContext;
    std::shared_ptr<Compute_Pipeline> m_pipeline;
    std::shared_ptr<ShaderModule> m_shader_module;
    std::vector<std::shared_ptr<DescriptorSet>> m_sets;
    // std::shared_ptr<DescriptorPool> m_descriptorSetPool;
    // std::shared_ptr<DescriptorSet> m_descriptorSet;
    // std::vector<std::shared_ptr<DescriptorSetTargetBase>> m_descriptorSetTarget;
};

}
