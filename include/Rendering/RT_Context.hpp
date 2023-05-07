#pragma once
#include "Rendering/Context_base.hpp"
#include "Wrapper/CommandBuffer.hpp"
// #include "Wrapper/Pipeline/Pipeline_base.hpp"
// #include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Helper/math.hpp"
#include <memory>
#include <vector>
// #include "shader/Data_struct.h"
#include "Helper/Shader_Data_Convert.h"


namespace MCRT {
class Device;
class Buffer;
class RenderPass;
class Framebuffer;
class Model;
class RT_Pipeline;
class Image;
// struct V_P_Matrix;
// class RenderTarget;
class RT_Context : public Context_base {
public:
    enum RT_Binding {
        e_tlas = 0,
        e_out_image = 1

    };
    enum Global_Binding {
        e_vp_matrix,

    };
    RT_Context(std::shared_ptr<Device> device);
    ~RT_Context();
    std::shared_ptr<RenderPass>& Get_render_pass() override
    {
        return m_renderpass;
    }
    std::shared_ptr<Pipeline_base> get_pipeline() override;

    std::shared_ptr<CommandBuffer> get_commandbuffer() override
    {
        if (!m_command_buffer)
            m_command_buffer.reset(new CommandBuffer);
        return m_command_buffer;
    }
    std::shared_ptr<Image> get_out_image() override;
    void prepare() override;
    void fill_render_targets() override;
    void Prepare_Framebuffer() override;
    void prepare_descriptorset() override;
    void prepare_pipeline() override;
    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit() override;
    void EndFrame() override;
    [[nodiscard("missing framebuffer")]] std::shared_ptr<Framebuffer>& get_framebuffer() override
    {
        return m_framebuffer;
    }
    void record_command(std::shared_ptr<CommandBuffer>) override;
    std::vector<std::shared_ptr<RenderTarget>>& Get_render_targets() override;
    void add_objs(std::vector<std::shared_ptr<Model>> objs);

private:
    void create_shader_bind_table();
    void create_uniform_buffer();

    std::shared_ptr<Uniform_Stuff<V_P_Matrix::shader_data>> vp_matrix;
    std::shared_ptr<RenderPass> m_renderpass;

    std::shared_ptr<Framebuffer> m_framebuffer;
    std::shared_ptr<CommandBuffer> m_command_buffer;
    std::vector<std::shared_ptr<RenderTarget>> render_targets;
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

    std::shared_ptr<Buffer> m_V_P_UBO;
};
}