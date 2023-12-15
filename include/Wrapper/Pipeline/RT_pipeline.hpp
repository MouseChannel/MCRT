#pragma once
#include "Wrapper/Pipeline/Pipeline_base.hpp"
namespace MCRT {
class ShaderModule;
class DescriptorSet;
class RT_Pipeline : public Pipeline_base {
public:
    enum {
        eRaygen,
        eMiss,
        eMiss2,
        eMiss3,
        eClosestHit,
        eClosestHit2,

        eClosestHit3,
        eAnyHit,
        eShaderGroupCount
    };
    RT_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shaders,
                std::vector<std::shared_ptr<DescriptorSet>> sets,
                int push_constants_size);
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