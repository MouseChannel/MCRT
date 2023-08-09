#pragma once
#include "Helper/Instance_base.hpp"
#include <exception>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Device;
class Instance;
class Window;
class Surface;
class SwapChain;
class CommandPool;
class Mesh;
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

class Context {
public:
    static std::unique_ptr<Context> _instance;
    static std::unique_ptr<Context>& Get_Singleton()
    {
        if (!_instance) {
            throw std::runtime_error("fail Context");
        }
        return _instance;
    }

    [[nodiscard("Missing Instance")]] auto& get_instance()
    {
        return m_instance;
    }
    [[nodiscard("missing window")]] std::shared_ptr<Window> get_window()
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

    virtual std::shared_ptr<RT_Context> get_rt_context() = 0;

    virtual std::shared_ptr<Compute_Context> get_compute_context() = 0;

    virtual std::shared_ptr<RenderContext> get_graphic_context() = 0;

    [[nodiscard("missing camera")]] auto& get_camera()
    {
        return m_camera;
    }
    [[nodiscard("missing renderpass")]] std::shared_ptr<RenderPass> get_renderpass();
    // virtual void init(std::shared_ptr<Window>);
    virtual std::shared_ptr<CommandBuffer> Begin_Frame() = 0;
    virtual void EndFrame() = 0;
    auto get_cur_frame_id()
    {
        return frame_id;
    }
    void reset()
    {
        frame_id = 0;
    }
    auto get_extent2d()
    {
        return extent2d;
    }
    void set_extent2d(int width, int height)
    {
        extent2d.width = width;
        extent2d.height = height;
    }

    virtual void prepare(std::shared_ptr<Window> window);
    virtual void re_create_context() = 0;

protected:
    vk::Extent2D extent2d;
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
    bool enable_filter = false;
    int frame_id = 0;
};

} // namespace MCRT