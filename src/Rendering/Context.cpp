#include "Rendering/Context.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include "Wrapper/Window_Surface.hpp"

namespace MCRT {

void Context::init(std::shared_ptr<Window> window)
{
    m_window = window;
    m_instance.reset(new Instance);
    m_surface.reset(new Surface);
    m_device.reset(new Device);
    m_swapchain.reset(new SwapChain);
    m_command_pool.reset(new CommandPool);
    m_debugger.reset(new Debugger);
    m_sampler.reset(new Sampler);
    // m_debugger->set_buffer_name(m_command_pool, "rer");

    m_model.reset(new Model("D:/MoChengRT/assets/model.obj", "D:/MoChengRT/assets/model.png"));

    m_as_builder.reset(new AS_Builder);
    m_as_builder->add_blas_obj(m_model);

    m_as_builder->build_blas();
    m_as_builder->build_tlas();

    int a = 0;
}
} // namespace MCRT