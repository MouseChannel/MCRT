#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class Sampler : public Component<vk::Sampler, Sampler> {
public:
    Sampler();
    ~Sampler();

private:
};
} // namespace MoCheng3D