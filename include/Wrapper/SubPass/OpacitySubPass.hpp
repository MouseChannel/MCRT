#pragma once
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "Rendering/Render_Target/DepthTarget.hpp"
namespace MCRT{


class OpacitySubPass :public BaseSubPass {
public:
    OpacitySubPass();
    void run_subpass() override;
    void set_description() override;
//    void prepare_renderTarget()override;
private:
};
}