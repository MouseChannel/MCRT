#pragma once
#include "Wrapper/Pipeline/Pipeline_base.hpp"
namespace MCRT {
class ShaderModule;

class RT_Pipeline : public Pipeline_base {
public:
    enum {
        eRaygen,
        eMiss,
        eMiss2,
        eMiss3,
        eClosestHit,
        eClosestHit2,
        eShaderGroupCount
    };
    RT_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shaders);
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