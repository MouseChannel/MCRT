#include "example/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    Context::Get_Singleton()->init(window);
    imgui.reset(new ImGuiContext);
    
    imgui->Init(window);
    struct MyStruct1 {
        int s;
    } ss;
    // UniformManager::make_uniform(ss, vk::ShaderStageFlagBits::eAll, vk::DescriptorType::eUniformBuffer);
}
void App::run()
{
    auto& context = Context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd);
        context->EndFrame();
        // context

        //   window.s
    }
}
} // namespace MCRT