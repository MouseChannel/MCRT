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
class Sampler;
class Debugger;
class AccelerationStructure_Bottom;
class AS_Builder;
class RT_Pipeline;
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

private:
    std::shared_ptr<Device> m_device;
    std::shared_ptr<Instance> m_instance;
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Surface> m_surface;
    std::shared_ptr<SwapChain> m_swapchain;
    std::shared_ptr<CommandPool> m_command_pool;
    std::shared_ptr<Debugger> m_debugger;
    std::shared_ptr<Sampler> m_sampler;
    std::shared_ptr<Model> m_model;
    std::vector<std::shared_ptr<AccelerationStructure_Bottom>> m_accelerate_structures;
    std::shared_ptr<AS_Builder> m_as_builder;
    std::shared_ptr<RT_Pipeline> m_rt_pipeline;
};

} // namespace MCRT