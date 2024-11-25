

#pragma once
#include "Wrapper/GraphicPass/GraphicPass.hpp"
namespace MCRT {

class UIPass : public GraphicPass {
public:
    void Init();
    void DrawUI(vk::CommandBuffer cmd,
                std::function<void()> func);
};
}