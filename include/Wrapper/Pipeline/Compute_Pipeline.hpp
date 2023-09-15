#pragma once
#include "Wrapper/Pipeline/Pipeline_base.hpp"
namespace MCRT {
class ShaderModule;
class DescriptorSet;
class Compute_Pipeline : public Pipeline_base {

public:
    Compute_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shaders, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size);
    vk::PipelineLayout get_layout()
    {
        return layout;
    }

private:
    vk::PipelineLayout layout;
};

}