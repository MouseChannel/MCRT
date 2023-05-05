#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {
class RenderPass : public Component<vk::RenderPass, RenderPass> {
public:
    RenderPass();
    ~RenderPass();

private:
};

}