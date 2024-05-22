#pragma once
// #include "Rendering/Render_Target/DepthTarget.hpp"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
namespace MCRT {

class ToneMapSubPass : public BaseSubPass {
public:
    ToneMapSubPass(std::weak_ptr<GraphicContext> graphicContext);
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    void make_offscreen_mesh();
    int get_DescriptorSetCount() override
    {
        return 3;
    }
    void recreate()override;

private:
};
}