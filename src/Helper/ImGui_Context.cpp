#include "Helper/ImGui_Context.hpp"

#include "Helper/CommandManager.hpp"
#include "Rendering/GLFW_Window.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
ImGuiContext::ImGuiContext()
{
}
void ImGuiContext::create_descriptor_pool()
{
    using type = vk::DescriptorType;
    vk::DescriptorPoolSize pool_sizes[] = { { type::eSampler, 1000 },
                                            { type::eCombinedImageSampler, 1000 },
                                            { type::eStorageImage, 1000 },
                                            { type::eUniformTexelBuffer, 1000 },
                                            { type::eStorageTexelBuffer, 1000 },
                                            { type::eUniformBuffer, 1000 },
                                            { type::eStorageBuffer, 1000 },
                                            { type::eUniformBufferDynamic, 1000 },
                                            { type::eStorageBufferDynamic, 1000 },
                                            { type::eInputAttachment, 1000 } };
    vk::DescriptorPoolCreateInfo create_info;
    create_info.setPoolSizes(pool_sizes)
        .setPoolSizeCount(IM_ARRAYSIZE(pool_sizes))
        .setMaxSets(1000 * IM_ARRAYSIZE(pool_sizes))
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    descriptor_pool = Context::Get_Singleton()->get_device()->get_handle().createDescriptorPool(create_info);
}
void ImGuiContext::Init(std::shared_ptr<Window> window)
{
    create_descriptor_pool();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls }
    ImGui::StyleColorsDark();

    auto& context = Context::Get_Singleton();
    ImGui_ImplGlfw_InitForVulkan(window->get_handle(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = context->get_instance()->get_handle();
    init_info.PhysicalDevice = context->get_device()->Get_Physical_device();
    init_info.Device = context->get_device()->get_handle();
    init_info.QueueFamily = context->get_device()->get_queuefamily_index().graphic_queue.value();
    init_info.Queue = context->get_device()->Get_Graphic_queue();
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = descriptor_pool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = context->get_swapchain()->Get_Swapchain_Image_size();
    init_info.MSAASamples = (VkSampleCountFlagBits)vk::SampleCountFlagBits::e1;
    // (VkSampleCountFlagBits)context->get_device()->Get_sampler_count();
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&init_info, context->get_renderpass()->get_handle());
    // upload font
    {

        CommandManager::ExecuteCmd(context->get_device()->Get_Graphic_queue(), [&](auto cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}
void ImGuiContext::Update(std::shared_ptr<CommandBuffer> cmd, std::function<void()> func)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    int c = 1;
    ImGui::NewFrame();
    {
      
        ImGui::Begin("its imgui window"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        func();
       
        ImGui::End();
    }
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmd->get_handle());
}
ImGuiContext::~ImGuiContext()
{
    Context::Get_Singleton()->get_device()->get_handle().destroyDescriptorPool(descriptor_pool);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
}