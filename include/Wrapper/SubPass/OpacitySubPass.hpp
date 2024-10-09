#pragma once
// #include "Rendering/Render_Target/DepthTarget.hpp"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
namespace MCRT {

class OpacitySubPass : public BaseSubPass {
public:
    OpacitySubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index);
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    int get_DescriptorSetCount() override
    {
        return 1;
    }

private:
};
}