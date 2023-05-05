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
class CommandBuffer;
class Graphic_Pipeline;
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
    std::vector<std::shared_ptr<RenderTarget>>& Get_render_targets() override
    {
        return render_frames[0]->Get_render_targets();
    }
    std::shared_ptr<Pipeline_base> get_pipeline() override;
    std::shared_ptr<RenderPass>& Get_render_pass() override
    {
        return m_renderpass;
    }
    std::shared_ptr<Image> get_out_image() override
    {
        return nullptr;
    }
    std::shared_ptr<CommandBuffer> get_commandbuffer() override
    {
        return command_buffer;
    }
    std::shared_ptr<Framebuffer>& get_framebuffer() override;
    // void Prepare();
    void prepare() override;
    void fill_render_targets() override;
    void Prepare_Framebuffer() override;
    void prepare_descriptorset() override;
    void prepare_pipeline() override;
    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit() override;
    void EndFrame() override;
    void record_command(std::shared_ptr<CommandBuffer> cmd) override;

private:
    bool enable_swapchain { false };
    int render_frame_count { 1 };
    std::shared_ptr<Device> m_device;
    std::shared_ptr<SwapChain> m_swapchain;
    std::vector<std::vector<std::shared_ptr<RenderTarget>>> all_rendertargets;
    std::vector<std::unique_ptr<RenderFrame>> render_frames;
    // void Prepare_RenderPass(std::vector<std::shared_ptr<RenderTarget>>& render_targets);
    std::shared_ptr<RenderPass> m_renderpass;
    std::shared_ptr<Graphic_Pipeline> m_graphic_pipeline;
    std::vector<std::shared_ptr<Fence>> fences;
    uint32_t current_frame { 0 };
    uint32_t current_index { 0 };
    std::shared_ptr<CommandBuffer> command_buffer;
    std::shared_ptr<CommandBuffer> Begin_Record_Command_Buffer();
    void End_Record_Command_Buffer();
};

}