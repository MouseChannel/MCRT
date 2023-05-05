#pragma once
#include "Helper/Instance_base.hpp"
#include <memory>
#include <vector>
namespace MCRT {
class Device;
class Instance;
class Window;
class Surface;
class SwapChain;
class CommandPool;
class Model;
class CommandBuffer;
class RenderContext;
class Sampler;
class Image;
class Debugger;
class AccelerationStructure_Bottom;
class AS_Builder;
class RT_Pipeline;
class Context_base;
class RenderPass;
class RT_Context;
// class RenderC
class Context : public Instance_base<Context> {
public:
    //   Context() = default;
    //   ~Context() = default;
    void init(std::shared_ptr<Window>);
    [[nodiscard("Missing Instance")]] auto& get_instance()
    {
        return m_instance;
    }
    [[nodiscard("missing window")]] auto& get_window()
    {
        return m_window;
    }
    [[nodiscard("missing surface")]] auto& get_surface()
    {
        return m_surface;
    }
    [[nodiscard("missing device")]] auto& get_device()
    {
        return m_device;
    }
    [[nodiscard("missing window_surface")]] auto& get_window_surface()
    {
        return m_surface;
    }
    [[nodiscard("missing swapchain")]] auto& get_swapchain()
    {
        return m_swapchain;
    }
    [[nodiscard("missing Sampler")]] auto& get_sampler()
    {
        return m_sampler;
    }
    [[nodiscard("missinf command_pool")]] auto& get_commandpool()
    {
        return m_command_pool;
    }
    [[nodiscard("missing debugger")]] auto& get_debugger()
    {
        return m_debugger;
    }
    [[nodiscard("missing rt_pipeline")]] auto& get_rt_pipeline()
    {
        return m_rt_pipeline;
    }
    auto& get_obj()
    {
        return m_model;
    }
    std::shared_ptr<Image> get_out_image();

    [[nodiscard("missing renderpass")]] std::shared_ptr<RenderPass> get_renderpass();
    std::shared_ptr<CommandBuffer> Begin_Frame();
void EndFrame();
    std::shared_ptr<CommandBuffer> BeginGraphicFrame();
    std::shared_ptr<CommandBuffer> BeginRTFrame();

    void EndGraphicFrame();
    void EndRTFrame();

private:
    std::shared_ptr<Device> m_device;
    std::shared_ptr<Instance> m_instance;
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Surface> m_surface;
    std::shared_ptr<SwapChain> m_swapchain;
    std::shared_ptr<RenderPass> m_renderpass;
    std::shared_ptr<CommandPool> m_command_pool;
    std::shared_ptr<CommandBuffer> m_command_buffer;
    std::shared_ptr<Debugger> m_debugger;
    std::shared_ptr<Sampler> m_sampler;
    std::shared_ptr<Model> m_model;
    std::vector<std::shared_ptr<AccelerationStructure_Bottom>> m_accelerate_structures;
    std::shared_ptr<AS_Builder> m_as_builder;
    std::shared_ptr<RT_Pipeline> m_rt_pipeline;
    std::shared_ptr<RenderContext> m_render_context;
    std::shared_ptr<RT_Context> m_RT_context;
    std::vector<std::shared_ptr<Context_base>> contexts;
};

} // namespace MCRT