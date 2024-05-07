#pragma once
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "Rendering/Render_Target/DepthTarget.hpp"
namespace MCRT{


class IMGUISubPass :public BaseSubPass {
public:
    IMGUISubPass(std::weak_ptr<GraphicContext> graphicContext);
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
private:
};
}