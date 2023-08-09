#pragma once

#include "Rendering/Render_Frame.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Rendering/Context_base.hpp"
namespace MCRT {
class Device;
class SwapChain;
class RenderFrame;
class RenderPass;
class Fence;
class ShaderModule;
class CommandBuffer;
class Graphic_Pipeline;
class Buffer;
class RenderContext : public Context_base {

public:
    RenderContext(std::shared_ptr<Device> device);
    ~RenderContext();

    [[nodiscard("missing Render_Frame")]] auto&
    Get_RenderFrame(uint32_t index)
    {
        return render_frames[index];
    }
    [[nodiscard("Missing semaphore")]] auto& Get_cur_render_semaphore()
    {
        return Get_RenderFrame(current_frame)->Get_render_semaphore();
    }
    [[nodiscard("Missing semaphore")]] auto& Get_cur_present_semaphore()
    {
        return Get_RenderFrame(current_frame)->Get_present_semaphore();
    }
    [[nodiscard]] auto Get_cur_index()
    {
        return current_index;
    }
    [[nodiscard("missing fence")]] auto& Get_cur_fence()
    {
        return fences[current_frame];
    }
    // [[nodiscard]] auto get_swapchain()
    // {
    // }
    std::vector<std::shared_ptr<RenderTarget>>& Get_render_targets()
    {
        return all_rendertargets[0];
    }
    void re_create() override;
    std::shared_ptr<Pipeline_base> get_pipeline() override;
    std::shared_ptr<Pipeline_base> get_pipeline2();

    std::shared_ptr<RenderPass>& Get_render_pass()
    {
        return m_renderpass;
    }
    std::shared_ptr<CommandBuffer> get_commandbuffer() override
    {
        return command_buffer;
    }
    std::shared_ptr<Framebuffer>& get_framebuffer();
    void Prepare_RenderPass();
    void prepare() override;
    void post_prepare() override;
    void fill_render_targets();
    void Prepare_Framebuffer();
    void re_create_swapchain();
    void prepare_descriptorset(std::function<void()> prepare_func) override;
    void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules) override;
    void prepare_pipeline2(std::vector<std::shared_ptr<ShaderModule>> shader_modules);
    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit() override;
    void EndFrame() override;
    void record_command(std::shared_ptr<CommandBuffer> cmd) override;

private:
    bool enable_swapchain { true };
    int render_frame_count { 1 };
    std::shared_ptr<Device> m_device;
    std::shared_ptr<SwapChain> m_swapchain;
    std::vector<std::vector<std::shared_ptr<RenderTarget>>> all_rendertargets;
    std::vector<std::unique_ptr<RenderFrame>> render_frames;
    std::shared_ptr<RenderPass> m_renderpass;
    std::shared_ptr<Graphic_Pipeline> m_graphic_pipeline;
    std::shared_ptr<Graphic_Pipeline> m_skybox_pipeline;
    std::shared_ptr<ShaderModule> vert_shader, frag_shader;
    std::vector<std::shared_ptr<Fence>> fences;
    uint32_t current_frame { 0 };
    uint32_t current_index { 0 };
    std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
    std::shared_ptr<CommandBuffer> command_buffer;

    std::shared_ptr<CommandBuffer> Begin_Record_Command_Buffer();
    void End_Record_Command_Buffer();
};

}