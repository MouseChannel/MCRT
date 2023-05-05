#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class Fence : public Component<vk::Fence, Fence> {
public:
    Fence();
    ~Fence();

private:
};
} // namespace MoCheng3D