#pragma once

#include "Rendering/Render_Target/Gbuffer_RenderTarget.hpp"
#include "Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
#include "Wrapper/SwapChain.hpp"
namespace MCRT {

class GbufferSubPass : public BaseSubPass {
public:
    GbufferSubPass(std::weak_ptr<GraphicContext> graphicContext,  int subpass_index,std::vector<std::shared_ptr<GBuffer_RenderTarget>> render_Targets);
    GbufferSubPass(std::weak_ptr<GraphicContext> graphicContext,  int subpass_index);
    
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    int get_DescriptorSetCount() override
    {
        //        return 3;
        return Context::Get_Singleton()->get_swapchain()->Get_Swapchain_Image_size();
    }
    void recreate()override;
private:
    std::vector<std::shared_ptr<GBuffer_RenderTarget>> m_render_Targets;
};
}