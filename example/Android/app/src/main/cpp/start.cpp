//#include "App.hpp"
#include "Rendering/AppWindow.hpp"
#include "raster_pbr_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/ImGui_Context.hpp"
#include <android/log.h>
#include <android_native_app_glue.h>
#include <iostream>
#include <jni.h>
#include <string>

static const char *kTAG = "mocheng";
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))
extern "C" {
android_app *app;

std::shared_ptr<MCRT::raster_context_pbr> context;
std::shared_ptr<MCRT::Window> m_window;
std::unique_ptr<MCRT::ImGuiContext> imgui;
bool isready = false;

void initVulkan(android_app *pApp);
void terminate();
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
//            context.reset(new MCRT::raster_context_pbr);
            LOGE("mocheng");
            app = pApp;
            m_window.reset(new MCRT::Window(app->window));
            MCRT::raster_context_pbr::Get_Singleton()->set_Android_app(app);
            MCRT::raster_context_pbr::Get_Singleton()->prepare(m_window);
            imgui.reset(new MCRT::ImGuiContext);
            imgui->Init(m_window);
            isready = true;
            break;

        case APP_CMD_TERM_WINDOW:
            terminate();


            break;

    }
}
void initVulkan(android_app *pApp) {
//    MoCheng3D::Context::Get_Singleton()->Init_Vulkan(pApp);
//    vulkan_loader->Init(pApp);
}
int32_t handleAppInput(struct android_app *app, AInputEvent *event) {
    std::string a = std::to_string(AInputEvent_getType(event));
    auto rr = a.c_str();
    MCRT::raster_context_pbr::Get_Singleton()->get_camera()->handleAppInput(app, event);
    ImGui_ImplAndroid_HandleInputEvent(event);

    return 1;

}

void terminate() {
    std::cout << "terminate" << std::endl;
}
void android_main(struct android_app *pApp) {
//    vulkan_loader.reset(new VulkanLoader);
    app = pApp;
    pApp->onAppCmd = handle_cmd;

    int events;
    android_poll_source *pSource;
    do {
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
        }
//        if (vulkan_loader->isready) {
//            vulkan_loader->Render_Loop();
//        }
        if (isready) {
            pApp->onInputEvent = handleAppInput;
//            bool r = false;
            auto cmd = MCRT::raster_context_pbr::Get_Singleton()->Begin_Frame();
            imgui->Update(cmd, [&]() {
                ImGui::Text("im a sentence!");
//                ImGui::SliderFloat("light_pos_x", &raster_context_pbr::light_pos_x, -2.0f, 2.0f);
//                ImGui::SliderFloat("light_pos_y", &raster_context_pbr::light_pos_y, -2.0f, 2.0f);
//                ImGui::SliderFloat("light_pos_z", &raster_context_pbr::light_pos_z, 0.f, 20.0f);
                ImGui::Checkbox("use_normal_map", &MCRT::raster_context_pbr::use_normal_map);
                ImGui::Checkbox("rm", &MCRT::raster_context_pbr::use_r_rm_map);
            });
            MCRT::raster_context_pbr::Get_Singleton()->EndFrame();
        }
    } while (!pApp->destroyRequested);
}
}