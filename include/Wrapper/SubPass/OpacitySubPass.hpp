#pragma once
#include "Rendering/Render_Target/DepthTarget.hpp"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
namespace MCRT {

class OpacitySubPass : public BaseSubPass {
public:
    OpacitySubPass(std::weak_ptr<GraphicContext> graphicContext);
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    // void draw(vk::CommandBuffer& cmd,std::vector<std::shared_ptr<Mesh>>&  meshs,void* push_constant) override;
    // void draw1(vk::CommandBuffer& cmd) override;

private:
};
}