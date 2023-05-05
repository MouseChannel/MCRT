#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
// #include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class Semaphore : public Component<vk::Semaphore, Semaphore> {
public:
    Semaphore();
    ~Semaphore();

private:
};
} // namespace MoCheng3D