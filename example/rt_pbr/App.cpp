#include "example/rt_pbr/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "example/rt_pbr/rt_pbr_context.hpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(1200, 800));
    rt_pbr_context::Get_Singleton()->prepare(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
}
void App::run()
{
    auto& context = rt_pbr_context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd, [&]() {  ImGui::Text("im a sentence!");
            ImGui::SliderFloat("light_pos_x", &rt_pbr_context::light_pos_x, -2.0f, 2.0f);
            ImGui::SliderFloat("light_pos_y", &rt_pbr_context::light_pos_y, -2.0f, 2.0f);
            ImGui::SliderFloat("light_pos_z", &rt_pbr_context::light_pos_z, 0.f, 20.0f);
            ImGui::SliderFloat("roughness", &rt_pbr_context::roughness, 0.01f, 1.0f);
            ImGui::SliderFloat("metallicness", &rt_pbr_context::met, 0.01f, 1.0f);
            ImGui::Text("counter = %d", Context::Get_Singleton()->get_enable_filter()); });
        context->EndFrame();
    }
}
} // namespace MCRT