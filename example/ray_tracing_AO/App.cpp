#include "example/ray_tracing_AO/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "example/ray_tracing_AO/raytracing_AO_Context.hpp"


namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    raytracing_AO_context::Get_Singleton()->prepare(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
}
void App::run()
{
    auto& context = raytracing_AO_context::Get_Singleton();
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