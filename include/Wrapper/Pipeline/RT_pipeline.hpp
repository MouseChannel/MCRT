#pragma once
#include "Wrapper/Pipeline/Pipeline_base.hpp"
namespace MCRT {
class ShaderModule;
class RT_Pipeline : public Pipeline_base {
public:
    RT_Pipeline();
    ~RT_Pipeline();
    vk::PipelineLayout get_layout() override
    {
        return layout;
    }

private:
    std::vector<std::shared_ptr<ShaderModule>> shader_modules;
    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> groups;
    vk::PipelineLayout layout;
};
}