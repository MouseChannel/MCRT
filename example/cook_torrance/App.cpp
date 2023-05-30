#include "example/cook_torrance/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "example/cook_torrance/cook_torrance_Context.hpp"

#include "Rendering/RT_Context.hpp"
namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    cook_torrance_context::Get_Singleton()->prepare(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
}
void App::run()
{

    auto& context = cook_torrance_context::Get_Singleton();

    // cook_torrance_context cook_torrance_context = std::reinterpret_pointer_cast<cook_torrance_context>(context);
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd, [&]() {
            ImGui::Text("im a sentence!");

            auto before = cook_torrance_context::roughness + cook_torrance_context::metallicness;
            ImGui::SliderFloat("roughness", &cook_torrance_context::roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("metallicness", &cook_torrance_context::metallicness, 0.0f, 1.0f);
            if (std::abs(before - cook_torrance_context::roughness - cook_torrance_context::metallicness) > 1e-6) {
                std::cout << "change" << std::endl;
                Context::Get_Singleton()->reset();
            }
            if (Context::Get_Singleton()->get_enable_filter()) {
                if (ImGui::Button("Disable filter")) {
                    Context::Get_Singleton()->set_enable_filter(false);
                }
            } else {
                if (ImGui::Button("Enable filter")) {
                    Context::Get_Singleton()->set_enable_filter(true);
                }
            }
            ImGui::Text("counter = %d", Context::Get_Singleton()->get_enable_filter());
            // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", io.DeltaTime, 1.0f / io.DeltaTime);
        });

        context->EndFrame();
    }
}
} // namespace MCRT