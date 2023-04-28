#include "Rendering/Context.hpp"
#include "Helper/Debugger.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Window_Surface.hpp"

namespace MCRT {

void Context::init(std::shared_ptr<Window> window)
{
    m_window = window;
    m_instance.reset(new Instance);
    m_surface.reset(new Surface);
    m_device.reset(new Device);
    m_command_pool.reset(new CommandPool);
    m_debugger.reset(new Debugger);
    // m_debugger->set_buffer_name(m_command_pool, "rer");

    m_model.reset(new Model("D:/MoChengRT/assets/model.obj", "D:/MoChengRT/assets/model.png"));

    m_as_builder.reset(new AS_Builder(m_device));
    auto as_geom = m_as_builder->object_to_vkGeometryKHR(m_model);
    m_as_builder->add_blas_obj(as_geom);
    m_as_builder->build_blas();
    int a = 0;
}
} // namespace MCRT