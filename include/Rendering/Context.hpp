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
class Camera;
class Context_base;
class RenderPass;
class Compute_Context;
class RT_Context;
// class RenderC
class Context : public Instance_base<Context> {
public:
    //   Context() = default;
    //   ~Context() = default;
    enum Context_index { Graphic,
                         Ray_tracing,
                         Compute };
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
    void set_swapchain(std::shared_ptr<SwapChain> swapchain)
    {
        m_swapchain = swapchain;
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
    void set_enable_filter(bool enable)
    {
        enable_filter = enable;
    }
    auto get_enable_filter()
    {
        return enable_filter;
    }
    std::shared_ptr<Image> get_out_image();

    auto get_rt_context()
    {
        auto base = contexts[Context_index::Ray_tracing];
        if (auto context = std::reinterpret_pointer_cast<RT_Context>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }
    auto get_compute_context()
    {
        auto base = contexts[Context_index::Compute];
        if (auto context = std::reinterpret_pointer_cast<Compute_Context>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not compute context");
    }

    auto get_graphic_context()
    {
        auto base = contexts[Context_index::Graphic];
        if (auto context = std::reinterpret_pointer_cast<RenderContext>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }
    [[nodiscard("missing camera")]] auto& get_camera()
    {
        return m_camera;
    }
    [[nodiscard("missing renderpass")]] std::shared_ptr<RenderPass> get_renderpass();
    std::shared_ptr<CommandBuffer> Begin_Frame();
    void EndFrame();
    std::shared_ptr<CommandBuffer> BeginGraphicFrame();
    std::shared_ptr<CommandBuffer> BeginComputeFrame();
    std::shared_ptr<CommandBuffer> BeginRTFrame();

    void EndGraphicFrame();
    void EndComputeFrame();
    void EndRTFrame();

private:
    std::shared_ptr<Device> m_device;
    std::shared_ptr<Instance> m_instance;
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Surface> m_surface;
    std::shared_ptr<SwapChain> m_swapchain;
    std::shared_ptr<CommandPool> m_command_pool;
    std::shared_ptr<Debugger> m_debugger;
    std::shared_ptr<Sampler> m_sampler;
    std::vector<std::shared_ptr<Context_base>> contexts;
    std::shared_ptr<Camera> m_camera;
    bool enable_filter;
};

} // namespace MCRT