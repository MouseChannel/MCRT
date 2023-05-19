#pragma once
#include "Wrapper/Pipeline/Pipeline_base.hpp"
namespace MCRT {
class ShaderModule;
class Compute_Pipeline : public Pipeline_base {

public:
    Compute_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shaders);
    vk::PipelineLayout get_layout()
    {
        return layout;
    }

private:
    vk::PipelineLayout layout;
};

}