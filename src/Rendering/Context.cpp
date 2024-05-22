#include "Rendering/Context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
// #include "Helper/Ray_Tracing/RT_Manager.hpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/ComputeContext.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Ray_Tracing/AS_bottom.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include "Wrapper/Texture.hpp"
#include "Wrapper/Window_Surface.hpp"
#include "Rendering/PBR/IBL_Manager.hpp"
// #include "vulkan/vulkan_to_string.hpp"

namespace MCRT {

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
void Context::prepare(std::shared_ptr<Window> window)
{

    m_window = window;
    do_prepare();
}
#else

void Context::prepare(std::shared_ptr<Window> window)
{
    m_window = window;
    do_prepare();
}
#endif

void Context::do_prepare()
{
    m_instance.reset(new Instance);
    m_surface.reset(new Surface);
    m_device.reset(new Device);
    m_command_pool.reset(new CommandPool);

    m_debugger.reset(new Debugger);
    m_sampler.reset(new Sampler);
    m_camera.reset(new Camera);

    m_camera->init();
}

void Context::Quit()
{

    PASS.clear();
    m_camera.reset();
    m_sampler.reset();
    m_debugger.reset();
    m_command_pool.reset();
 
    m_swapchain.reset();
    Mesh::all_meshs.clear();
    Texture::textures.clear();
 
    // IBLManager::Get_Singleton()->Release();
    IBLManager::Get_Singleton().reset();

}
Context::~Context()
{
    m_device.reset();
    m_surface.reset();
    m_instance.reset();
}

std::shared_ptr<RenderPass> Context::get_renderpass()
{
    return get_graphic_context()->Get_render_pass();
}

} // namespace MCRT