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
            if (raytracing_AO_context::enable_ao) {
                if (ImGui::Button("Disable ao")) {
                    raytracing_AO_context::enable_ao = 0;
                    Context::Get_Singleton()->reset();
                }
                 if (ImGui::SliderInt("sample_count", &raytracing_AO_context::sample_count, 1, 100)) {
                Context::Get_Singleton()->reset();
                }
              if (ImGui::SliderFloat("ao_field", &raytracing_AO_context::ao_field, 0.001, 1)) {
                Context::Get_Singleton()->reset();
             }
            } else {
                if (ImGui::Button("Enable ao")) {
                    raytracing_AO_context::enable_ao = 1;
                    Context::Get_Singleton()->reset();
                }
            }

           
            // if (std::abs(before - cook_torrance_context::roughness - cook_torrance_context::metallicness) > 1e-6) {
            //     std::cout << "change" << std::endl;
            //     Context::Get_Singleton()->reset();
            // }

            ImGui::Text("counter = %d", Context::Get_Singleton()->get_enable_filter()); });
        context->EndFrame();
    }
}
} // namespace MCRT