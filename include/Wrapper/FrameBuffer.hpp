#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include "Wrapper/Image.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class RenderPass;
class Framebuffer : public Component<vk::Framebuffer, Framebuffer> {
public:
    Framebuffer(std::shared_ptr<RenderPass> renderpass, std::vector<std::shared_ptr<Image>> images);
    ~Framebuffer();

private:
};
} // namespace MoCheng3D