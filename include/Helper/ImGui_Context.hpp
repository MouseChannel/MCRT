#pragma once
#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_glfw.h"
#include "lib/imgui/imgui_impl_vulkan.h"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace MCRT {

class CommandBuffer;
class Window;
class ImGuiContext {
public:
    ImGuiContext();
    ~ImGuiContext();
    void Init(std::shared_ptr<Window> window);
    void Update(std::shared_ptr<CommandBuffer> cmd, std::function<void()> func);

private:
    vk::DescriptorPool descriptor_pool;
    void create_descriptor_pool();
    ImGuiIO io;
};
}