#pragma once
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
// #include "Rendering/Render_Target/SwapChainTarget.hpp"
// #include "Rendering/Render_Target/DepthTarget.hpp"

namespace MCRT {

class IMGUISubPass : public BaseSubPass {
public:
    IMGUISubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index);
    ~IMGUISubPass();
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    void drawUI(std::shared_ptr<CommandBuffer> cmd, std::function<void()> func);
    // void draw(vk::CommandBuffer& cmd, std::vector<std::shared_ptr<Mesh>>& meshs, void* push_constant) override;

private:
    vk::DescriptorPool descriptor_pool;
    void create_descriptor_pool();

    std::shared_ptr<RenderPass> imgui_renderpass;
};
}