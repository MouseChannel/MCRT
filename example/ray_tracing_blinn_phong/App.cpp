#include "example/path_tracing/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "example/ray_tracing_blinn_phong/blinn_phong_Context.cpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    blinn_phong_context::Get_Singleton()->init(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
}
void App::run()
{
    auto& context = blinn_phong_context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd, [&]() {  ImGui::Text("im a sentence!");
            ImGui::SliderFloat("light_pos_x", &blinn_phong_context::light_pos_x, -2.0f, 2.0f);
            ImGui::SliderFloat("light_pos_y", &blinn_phong_context::light_pos_y, -2.0f, 2.0f);
            ImGui::SliderFloat("light_pos_z", &blinn_phong_context::light_pos_z, 0.f, 20.0f);
            ImGui::Text("counter = %d", Context::Get_Singleton()->get_enable_filter()); });
        context->EndFrame();
    }
}
} // namespace MCRT