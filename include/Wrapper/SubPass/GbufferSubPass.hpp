#pragma once

#include "Rendering/Render_Target/Gbuffer_RenderTarget.hpp"
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
namespace MCRT {

class GbufferSubPass : public BaseSubPass {
public:
    GbufferSubPass(std::weak_ptr<GraphicContext> graphicContext,std::vector<std::shared_ptr<GBuffer_RenderTarget>> render_Targets);
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    // void draw(vk::CommandBuffer& cmd,std::vector<std::shared_ptr<Mesh>>&  meshs,void* push_constant) override;
    // void draw1(vk::CommandBuffer& cmd) override;

private:
    std::vector<std::shared_ptr<GBuffer_RenderTarget>> m_render_Targets;
};
}