#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {
class Pipeline_base : public Component<vk::Pipeline, Pipeline_base> {
public:
    [[nodiscard("missing pipeline_layout")]] virtual vk::PipelineLayout get_layout() = 0;
    std::vector<vk::DescriptorSet> get_descriptor_sets()
    {
        return descriptor_sets;
    }

protected:
    std::vector<vk::DescriptorSet> descriptor_sets;
};

}