#pragma once
#include "vulkan/vulkan.hpp"
namespace MCRT {
class SwapChainTarget {
public:
    SwapChainTarget();

private:
    vk::AttachmentDescription des;
};
}