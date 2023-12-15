#include "example/cloud/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/AppWindow.hpp"
#include "Rendering/Context.hpp"
#include "example/cloud/cloud_context.hpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(1200, 800));
    cloud_context::Get_Singleton()->prepare(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
}
void App::run()
{
    auto& context = cloud_context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd, [&]() {  ImGui::Text("im a sentence!");
            ImGui::SliderFloat("noise_scale", &cloud_context::noise_scale, 0.5f, 5.0f);
            ImGui::SliderFloat("thickness", &cloud_context::thickness, 0.5f, 3.0f);
//            ImGui::SliderFloat("light_pos_y", &cloud_context::light_pos_y, -2.0f, 2.0f);
//            ImGui::SliderFloat("light_pos_z", &cloud_context::light_pos_z, 0.f, 20.0f);
//            ImGui::SliderFloat("roughness", &cloud_context::roughness, 0.01f, 1.0f);
//            ImGui::SliderFloat("metallicness", &cloud_context::met, 0.01f, 1.0f);
//            ImGui::SliderInt("normal", &cloud_context::apply_normal, 0, 1);
//            ImGui::Checkbox("use_normal_map", &cloud_context::use_normal_map);
//            ImGui::Checkbox("use_diffuse_map", &cloud_context::use_abedo);
//            ImGui::Checkbox("use_RM_map", &cloud_context::use_RM_map);
//            ImGui::Text("counter = %d", Context::Get_Singleton()->get_enable_filter()); 
            });
        context->EndFrame();
    }
}
} // namespace MCRT