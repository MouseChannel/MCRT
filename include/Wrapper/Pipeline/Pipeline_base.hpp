#pragma once
#include "Wrapper/Component.hpp"
#include <memory>
namespace MCRT {
class DescriptorSet;
class Pipeline_base : public Component<vk::Pipeline, Pipeline_base> {
public:
    // [[nodiscard("missing pipeline_layout")]] virtual vk::PipelineLayout get_layout() = 0;
    // Pipeline_base(std::vector<std::shared_ptr<DescriptorSet>> sets);
    // Pipeline_base(std::shared_ptr<DescriptorSet> set);
    Pipeline_base() = default;
   
    // std::vector<vk::DescriptorSet> get_descriptor_sets()
    // {
    //     return descriptor_sets;
    // }
    // void set_descriptor_sets(std::vector<vk::DescriptorSet> sets)
    // {
    //     descriptor_sets = sets;
    // }
    // [[nodiscard]] auto get_pushconstants_size()
    // {
    //     return m_push_constants_size;
    // }

    void Make_Layout(vk::DescriptorSetLayout descriptorSetLayout, int pc_size, vk::ShaderStageFlags push_stage);
    auto get_layout(){
        return pipelineLayout;
    }
    virtual ~Pipeline_base() = default;

protected:
    // std::vector<vk::DescriptorSet> descriptor_sets;
    // std::vector<vk::DescriptorSetLayout> m_descriptor_layouts;
    vk::PipelineLayout pipelineLayout;
    // int m_push_constants_size;
};

}