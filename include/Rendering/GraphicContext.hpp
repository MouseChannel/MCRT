#pragma once

#include "Rendering/Render_Frame.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Rendering/BaseContext.hpp"
#include "Wrapper/SubPass/BaseSubPass.hpp"
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
class GraphicContext : public BaseContext {

public:
    // enum {
    //     SwapChainAttachment = 0,
    //     ColorAttachment,
    //     DepthAttachment,
    //     ResolveAttachment,
    //     AttachmentCount

    // };
    GraphicContext(std::shared_ptr<Device> device);
    // GraphicContext();
    ~GraphicContext();

    [[nodiscard("missing Render_Frame")]] auto&
    Get_RenderFrame(uint32_t index)
    {
        return render_frames[index];
    }
    [[nodiscard("Missing semaphore")]] auto& Get_cur_render_semaphore()
    {
        // return Get_RenderFrame(current_frame)->Get_render_semaphore();
        return Get_RenderFrame(current_index)->Get_render_semaphore();
    }
    [[nodiscard("Missing semaphore")]] auto& Get_cur_present_semaphore()
    {
        // return Get_RenderFrame(current_frame)->Get_present_semaphore();
        return Get_RenderFrame(current_index)->Get_present_semaphore();
    }

    [[nodiscard("missing fence")]] auto& Get_cur_fence()
    {
        // return fences[current_frame];
        return fences[current_index];
    }

    std::vector<std::shared_ptr<RenderTarget>>& Get_render_targets(int swapchain_frame = 0)
    {
        return all_rendertargets[swapchain_frame];
    }

    void re_create() override;
    // std::shared_ptr<Pipeline_base> get_pipeline() override;
    //    std::shared_ptr<Pipeline_base> get_pipeline2();

    std::shared_ptr<RenderPass>& Get_render_pass()
    {
        return m_renderpass;
    }
    std::shared_ptr<CommandBuffer> get_commandbuffer() override
    {
        return command_buffer;
    }
    auto get_cur_index()
    {
        return current_index;
    }
     void Prepare_RenderPass();
    void AddSubPassDependency(vk::SubpassDependency dependency);
    void prepare() override;
    void post_prepare() override;
    void fill_render_targets();
    void Prepare_Framebuffer();
    int AddSwapchainRenderTarget();
    int AddDepthRenderTarget();
    int AddColorRenderTarget();
    int AddResolveRenderTarget();
    int AddGbufferRenderTarget(vk::Format format);
    void re_create_swapchain();
    // void prepare_descriptorset(std::function<void()> prepare_func) override;
    // void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size) override;

    // void prepare_pipeline2(std::vector<std::shared_ptr<ShaderModule>> shader_modules);
    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Begin_RenderPass(std::shared_ptr<CommandBuffer> cmd);
    void Submit() override;
    void EndFrame() override;
    // void record_command(std::shared_ptr<CommandBuffer> cmd) override;
    auto& get_subpasses()
    {
        return subpasses;
    }
    std::vector<std::shared_ptr<BaseSubPass>> subpasses;
    int swapChainAttachmentindex, colorAttachmentIndex, depthAttachmentIndex, resolveAttachmentindex;

private:
    bool has_inited { false };
    bool enable_swapchain { true };

    int render_frame_count { 1 };
    std::shared_ptr<Device> m_device;
    std::shared_ptr<SwapChain> m_swapchain;
    std::vector<std::vector<std::shared_ptr<RenderTarget>>> all_rendertargets;
    std::vector<std::unique_ptr<RenderFrame>> render_frames;
    std::shared_ptr<RenderPass> m_renderpass;

    std::vector<std::shared_ptr<Fence>> fences;

    uint32_t current_index { 0 };

    std::shared_ptr<CommandBuffer> command_buffer;
    std::vector<vk::SubpassDependency> m_subpass_dependencies;

    std::shared_ptr<CommandBuffer> Begin_Record_Command_Buffer();
    void End_Record_Command_Buffer();
    // void AddRenderTarget(std::shared_ptr<RenderTarget> renderTarget);
};

}