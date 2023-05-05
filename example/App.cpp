#include "example/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    Context::Get_Singleton()->init(window);
    // imgui.reset(new ImGuiContext);
    // imgui->Init(window);
}
void App::run()
{
    auto& context = Context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        // imgui->Update(cmd);
        context->EndFrame();

        //   window.s
    }
}
} // namespace MCRT