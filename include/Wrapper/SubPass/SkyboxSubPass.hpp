#pragma once
#include "Rendering/Render_Target/DepthTarget.hpp"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
#include <functional>
namespace MCRT{


class SkyboxSubPass :public BaseSubPass {
public:
    SkyboxSubPass(std::shared_ptr<DescriptorManager> descriptorManager);
    void set_description() override;
//    void prepare_renderTarget()override;
    void run_subpass() override;
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
//    void  prepare_descriptorset(std::function<void()> prepare)override;
    void  prepare_pipeline() override;
private:};
}