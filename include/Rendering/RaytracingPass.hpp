#pragma once
#include "Rendering/BasePass.hpp"
#include "Wrapper/CommandBuffer.hpp"
// #include "Wrapper/Pipeline/Pipeline_base.hpp"
// #include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Helper/math.hpp"
#include "Rendering/Host_Uniform.hpp"
#include <memory>
#include <vector>
#include "example/base/shaders/ray_tracing/Data_struct.h"


namespace MCRT {
class Device;
class Buffer;
class RenderPass;
class Framebuffer;
class Mesh;
class RT_Pipeline;
class Image;
// struct V_P_Matrix;
// class RenderTarget;
class RaytracingPass : public BasePass {
public:
    RaytracingPass(std::shared_ptr<Device> device);
    RaytracingPass();

    std::shared_ptr<Pipeline_base> get_pipeline() override;

    std::shared_ptr<CommandBuffer> get_commandbuffer() override
    {
        if (!m_command_buffer)
            m_command_buffer.reset(new CommandBuffer);
        return m_command_buffer;
    }

    std::shared_ptr<Image> get_out_image();

    auto get_gbuffer()
    {
        return m_gbuffer;
    }

    void prepare() override;
    void post_prepare() override;
    void prepare_descriptorset(std::function<void()> prepare_func) override;
    void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size) override;
    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit() override;
    void EndFrame() override;

    void record_command(std::shared_ptr<CommandBuffer>) override;
    void re_create() override;
    // void re_create_descriptorset() override;
    void build_accelerate_structure();

    void set_hit_shader_count(int count)
    {
        hit_shader_count = count;
    }

    auto get_hit_shader_count()
    {
        return hit_shader_count;
    }
    void set_anyhit_shader_count(int count)
    {
        anyhit_shader_count = count;
    }

    auto get_anyhit_shader_count()
    {
        return anyhit_shader_count;
    }

    void set_miss_shader_count(int count)
    {
        miss_shader_count = count;
    }

    auto get_miss_shader_count()
    {
        return miss_shader_count;
    }

    void create_offscreen_image();

    auto& get_rgen_region()
    {
        return m_rgenRegion;
    }

    auto& get_hit_region()
    {
        return m_hitRegion;
    }

    auto& get_miss_region()
    {
        return m_missRegion;
    }

private:
    void create_shader_bind_table();
    void create_uniform_buffer();
    void update_ubo(std::shared_ptr<CommandBuffer> cmd);

    std::shared_ptr<Uniform_Stuff<Camera_data>> camera_data;
    std::shared_ptr<Uniform_Stuff<Address>> obj_data_address;
    std::vector<Address> m_objs_address;
    vk::Extent2D extent2d;
    std::shared_ptr<CommandBuffer> m_command_buffer;
    std::shared_ptr<Image> m_out_image;
    std::vector<std::shared_ptr<Image>> m_gbuffer;

    std::shared_ptr<Device> m_device;
    std::shared_ptr<RT_Pipeline> m_rt_pipeline;
    // shader bind table
    vk::StridedDeviceAddressRegionKHR m_rgenRegion;
    vk::StridedDeviceAddressRegionKHR m_missRegion;
    vk::StridedDeviceAddressRegionKHR m_hitRegion;
    vk::StridedDeviceAddressRegionKHR m_anyhitRegion;
    
    vk::StridedDeviceAddressRegionKHR m_callRegion;
    std::shared_ptr<Buffer> m_SBT_buffer;

    std::shared_ptr<Buffer> m_SBT_buffer_rgen;

    std::shared_ptr<Buffer> m_SBT_buffer_rmiss;

    std::shared_ptr<Buffer> m_SBT_buffer_rhit;
    std::shared_ptr<Buffer> m_SBT_buffer_rahit;

    int miss_shader_count{ 0 };
    int hit_shader_count{ 0 };
    int anyhit_shader_count{ 0 };
};
}
