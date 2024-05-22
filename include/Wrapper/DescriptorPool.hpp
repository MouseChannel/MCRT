#pragma once

#include "Wrapper/Component.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class DescriptorSet;
class DescriptorPool : public Component<vk::DescriptorPool, DescriptorPool> {
public:
    DescriptorPool(std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_size);
    DescriptorPool(std::vector<vk::DescriptorPoolSize> ci);

    ~DescriptorPool();
    // std::shared_ptr<DescriptorSet> CreateDescriptorSet();
private:
};
} // namespace MoCheng3D