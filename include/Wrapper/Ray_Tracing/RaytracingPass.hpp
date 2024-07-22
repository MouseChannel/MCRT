#pragma once
#include "Wrapper/BasePass.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
#include <functional>

namespace MCRT {

class RaytracingPass : public BasePass {
public:
    enum Stage {
        E_GENERATE,
        E_COLOESTHIT,
        E_MISS
    };
    RaytracingPass();
    // void prepare_pipeline() override;
    void prepare_pipeline(int pc_size) override;
    void SetShaderModule(std::shared_ptr<ShaderModule> shader_module, Stage stage);
    void SetShaderModule(std::string shader_path, Stage stage);
    void create_shader_bind_table();
    int get_DescriptorSetCount() override
    {
        return 1;
    }
    auto get_pipeline()
    {
        return m_pipeline;
    }
    void raytrace(vk::CommandBuffer cmd);

protected:
    std::shared_ptr<RT_Pipeline> m_pipeline;
    std::vector<std::shared_ptr<ShaderModule>> m_gen_shader_modules;

    std::vector<std::shared_ptr<ShaderModule>> m_hit_shader_modules;
    std::vector<std::shared_ptr<ShaderModule>> m_miss_shader_modules;

    std::vector<std::shared_ptr<ShaderModule>> m_shader_modules;

    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_groups;

    vk::StridedDeviceAddressRegionKHR m_rgenRegion;
    vk::StridedDeviceAddressRegionKHR m_missRegion;
    vk::StridedDeviceAddressRegionKHR m_hitRegion;
    vk::StridedDeviceAddressRegionKHR m_anyhitRegion;
};

}