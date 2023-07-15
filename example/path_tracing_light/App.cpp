#include "example/path_tracing/App.hpp"
#include "Helper/ImGui_Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "example/path_tracing_light/Path_tracing_light_Context.hpp"

namespace MCRT {
void App::init()
{
    window.reset(new Window(800, 800));
    Path_tracing_light_context::Get_Singleton()->prepare(window);
    imgui.reset(new ImGuiContext);

    imgui->Init(window);
}
void App::run()
{
    auto& context = Path_tracing_light_context::Get_Singleton();
    while (!window->Should_Close()) {
        window->PollEvents();
        auto cmd = context->Begin_Frame();
        imgui->Update(cmd, [&]() {
            ImGui::Text("im a sentence!");
            if (ImGui::Checkbox("enable_dir", &Path_tracing_light_context::enable_dir)) {
                Context::Get_Singleton()->reset();
            }
            if (ImGui::Checkbox("enable_indir", &Path_tracing_light_context::enable_indir)) {
                Context::Get_Singleton()->reset();
            }
            if (ImGui::Checkbox("enable_filter", &Path_tracing_light_context::enable_filter)) {
                Context::Get_Singleton()->reset();
            }
            if (ImGui::SliderFloat("light_desity", &Path_tracing_light_context::light_desity, 0.1f, 10.0f)) {
                Context::Get_Singleton()->reset();
            }
        });
        context->EndFrame();
    }
}
} // namespace MCRT