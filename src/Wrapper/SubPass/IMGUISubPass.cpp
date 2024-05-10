#include "Wrapper/SubPass/IMGUISubPass.hpp"
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_glfw.h"
#include "lib/imgui/imgui_impl_vulkan.h"

#include "Wrapper/Instance.hpp"

#include "Helper/CommandManager.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"
#include "Rendering/AppWindow.hpp"
#include "example/base/raster_context.hpp"

namespace MCRT {

IMGUISubPass::IMGUISubPass(std::weak_ptr<GraphicContext> graphicContext)
    : BaseSubPass(graphicContext)
{
}
void IMGUISubPass::prepare_vert_shader_module(std::string vert_shader)
{
}
void IMGUISubPass::prepare_frag_shader_module(std::string frag_shader)
{
}
void IMGUISubPass::prepare_pipeline(int pc_size)
{
}
void IMGUISubPass::post_prepare()
{
    create_descriptor_pool();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io { ImGui::GetIO() };

    io.IniFilename = NULL;
    io.LogFilename = NULL;
    //    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    auto& context = Context::Get_Singleton();

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    ImGui_ImplAndroid_Init(context->get_window()->get_handle());
#else

    ImGui_ImplGlfw_InitForVulkan(Context::Get_Singleton()->get_window()->get_handle(), true);
#endif
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = context->get_instance()->get_handle();
    init_info.PhysicalDevice = context->get_device()->Get_Physical_device();
    init_info.Device = context->get_device()->get_handle();
    init_info.QueueFamily = context->get_device()->get_queuefamily_index().graphic_queue.value();
    init_info.Queue = context->get_device()->Get_Graphic_queue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = VkDescriptorPool(descriptor_pool);
    init_info.Subpass = raster_context::IMGUISubPassIndex;
    init_info.MinImageCount = 2;
    init_info.ImageCount = context->get_swapchain()->Get_Swapchain_Image_size();
    init_info.MSAASamples = (VkSampleCountFlagBits)vk::SampleCountFlagBits::e1;
    // (VkSampleCountFlagBits)context->get_device()->Get_sampler_count();
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;

    ImGui_ImplVulkan_Init(&init_info, (VkRenderPass)Context::Get_Singleton()->get_renderpass()->get_handle());
    // upload font
    {
        // Load Fonts
        // io.Fonts->AddFontFromFileTTF("/home/mocheng/project/MCRT/OpenDyslexicAltNerdFont-Bold.otf", 25.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        io.Fonts->TexHeight = 20;
        CommandManager::ExecuteCmd(context->get_device()->Get_Graphic_queue(),
                                   [&](auto cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}
void IMGUISubPass::create_descriptor_pool()
{
    using type = vk::DescriptorType;
    std::vector<vk::DescriptorPoolSize> pool_sizes { { type::eSampler, 100 },
                                                     { type::eCombinedImageSampler, 100 },
                                                     { type::eStorageImage, 100 },
                                                     { type::eUniformTexelBuffer, 100 },
                                                     { type::eStorageTexelBuffer, 100 },
                                                     { type::eUniformBuffer, 100 },
                                                     { type::eStorageBuffer, 100 },
                                                     { type::eUniformBufferDynamic, 100 },
                                                     { type::eStorageBufferDynamic, 100 },
                                                     { type::eInputAttachment, 100 } };
    vk::DescriptorPoolCreateInfo create_info;
    create_info.setPoolSizes(pool_sizes)
        // .setPoolSizeCount(IM_ARRAYSIZE(pool_sizes))
        .setMaxSets(100 * pool_sizes.size())
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    descriptor_pool = Context::Get_Singleton()->get_device()->get_handle().createDescriptorPool(
        create_info);
}

void IMGUISubPass::drawUI(std::shared_ptr<CommandBuffer> cmd, std::function<void()> func)
{
    ImGui_ImplVulkan_NewFrame();
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    ImGui_ImplAndroid_NewFrame();
#else
    ImGui_ImplGlfw_NewFrame();
#endif

    ImGui::NewFrame();
    {

        ImGui::Begin(
            "setting"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        func();

        ImGui::End();
    }
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd->get_handle());
}
}
