#include "Rendering/Context.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/Ray_Tracing/RT_Manager.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
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

    m_model.reset(new Model("D:/MoChengRT/assets/model.obj",
                            "D:/MoChengRT/assets/model.png"));

    AS_Builder::Get_Singleton()->add_blas_obj(m_model);

    AS_Builder::Get_Singleton()->build_blas();
    AS_Builder::Get_Singleton()->build_tlas();
    create_rt_descriptor_set();

    m_rt_pipeline.reset(new RT_Pipeline);
    int a = 0;
}
} // namespace MCRT