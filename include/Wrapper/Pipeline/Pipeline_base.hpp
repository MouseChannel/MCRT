#pragma once
#include "Wrapper/Component.hpp"
#include <memory>
namespace MCRT {
class DescriptorSet;
class Pipeline_base : public Component<vk::Pipeline, Pipeline_base> {
public:
    [[nodiscard("missing pipeline_layout")]] virtual vk::PipelineLayout get_layout() = 0;
    Pipeline_base(std::vector<std::shared_ptr<DescriptorSet>> sets);
    std::vector<vk::DescriptorSet> get_descriptor_sets()
    {
        return descriptor_sets;
    }
    void set_descriptor_sets(std::vector<vk::DescriptorSet> sets)
    {
        descriptor_sets = sets;
    }

protected:
    std::vector<vk::DescriptorSet> descriptor_sets;
    std::vector<vk::DescriptorSetLayout> m_descriptor_layouts;
};

}