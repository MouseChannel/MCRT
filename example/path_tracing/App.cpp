#include "example/path_tracing/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "example/path_tracing/Path_tracing_Context.cpp"
#include "Rendering/GLFW_Window.hpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    Path_tracing_context::Get_Singleton()->init(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
  
}
void App::run()
{
    auto& context = Path_tracing_context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd, [&]() {  ImGui::Text("im a sentence!");
            // if (ImGui::Button("Close Me"))
            //     show_another_window = false;
            if (Context::Get_Singleton()->get_enable_filter()) {
                if (ImGui::Button("Disable filter")) {
                    Context::Get_Singleton()->set_enable_filter(false);
                }
            } else {
                if (ImGui::Button("Enable filter")) {
                    Context::Get_Singleton()->set_enable_filter(true);
                }
            }
            // if (ImGui::Button("Button"))
            //     c++;
            ImGui::Text("counter = %d", Context::Get_Singleton()->get_enable_filter()); });
        context->EndFrame();
         
    }
}
} // namespace MCRT