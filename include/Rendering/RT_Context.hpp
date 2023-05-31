#pragma once
#include "Rendering/Context_base.hpp"
#include "Wrapper/CommandBuffer.hpp"
// #include "Wrapper/Pipeline/Pipeline_base.hpp"
// #include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Helper/math.hpp"
#include "Rendering/Host_Uniform.hpp"
#include <memory>
#include <vector>

// #include "shader/Data_struct.h"
// #include "Helper/Shader_Data_Convert.h"

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
class RT_Context : public Context_base {
public:
    RT_Context(std::shared_ptr<Device> device);
    ~RT_Context();
    // auto& get_pushcontants_ray()
    // {
    //     return pushContant_Ray;
    // }
    // auto& get_cur_frame_id()
    // {
    //     return frame_id;
    // }
    std::shared_ptr<Pipeline_base> get_pipeline() override;

    std::shared_ptr<CommandBuffer> get_commandbuffer() override
    {
        if (!m_command_buffer)
            m_command_buffer.reset(new CommandBuffer);
        return m_command_buffer;
    }
    std::shared_ptr<Image> get_out_image();
    // auto get_normal_buffer()
    // {
    //     return m_normal_gbuffer;
    // }
    // auto get_position_buffer()
    // {
    //     return m_position_gbuffer;
    // }
    auto get_gbuffer()
    {
        return m_gbuffer;
    }
    void prepare(std::vector<std::shared_ptr<ShaderModule>> shader_modules) override;
    void post_prepare() override;
    void prepare_descriptorset(std::function<void()> prepare_func) override;
    void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules) override;
    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit() override;
    void EndFrame() override;
    // [[nodiscard("missing framebuffer")]] std::shared_ptr<Framebuffer>& get_framebuffer() override
    // {
    //     return m_framebuffer;
    // }
    void record_command(std::shared_ptr<CommandBuffer>) override;
    void build_accelerate_structure();
    void set_hit_shader_count(int count)
    {
        hit_shader_count = count;
    }
    auto get_hit_shader_count()
    {
        return hit_shader_count;
    }
    void set_miss_shader_count(int count)
    {
        miss_shader_count = count;
    }
    auto get_miss_shader_count()
    {
        return miss_shader_count;
    }
    // void set_constants_size(int size)
    // {
    //     push_constants_size = size;
    // }
    // auto get_constants_size()
    // {
    //     return push_constants_size;
    // }

private:
    void create_shader_bind_table();
    void create_uniform_buffer();
    void update_ubo(std::shared_ptr<CommandBuffer> cmd);

    std::shared_ptr<Uniform_Stuff<Camera_data>> camera_data;
    std::shared_ptr<Uniform_Stuff<Address>> obj_data_address;
    // std::shared_ptr<RenderPass> m_renderpass;
    std::vector<Address> m_objs_address;

    // std::shared_ptr<Framebuffer> m_framebuffer;
    std::shared_ptr<CommandBuffer> m_command_buffer;
    // std::vector<std::shared_ptr<RenderTarget>> render_targets;
    std::shared_ptr<Image> m_out_image;
    // gbuffer
    // std::shared_ptr<Image> m_position_gbuffer;
    // std::shared_ptr<Image> m_normal_gbuffer;
    std::vector<std::shared_ptr<Image>> m_gbuffer;

    // end gbuffer
    std::shared_ptr<Device> m_device;
    std::shared_ptr<RT_Pipeline> m_rt_pipeline;
    // shader bind table
    vk::StridedDeviceAddressRegionKHR m_rgenRegion;
    vk::StridedDeviceAddressRegionKHR m_missRegion;
    vk::StridedDeviceAddressRegionKHR m_hitRegion;
    vk::StridedDeviceAddressRegionKHR m_callRegion;
    std::shared_ptr<Buffer> m_SBT_buffer;

    std::shared_ptr<Buffer> m_SBT_buffer_rgen;

    std::shared_ptr<Buffer> m_SBT_buffer_rmiss;

    std::shared_ptr<Buffer> m_SBT_buffer_rhit;
    // int push_constants_size;

    int miss_shader_count { 0 };
    int hit_shader_count { 0 };
};
}