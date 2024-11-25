#pragma once

#include "Wrapper/Component.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MCRT {
class DescriptorSet;
class DescriptorPool : public Component<vk::DescriptorPool, DescriptorPool> {
public:
    DescriptorPool(std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_size);
    DescriptorPool(std::vector<vk::DescriptorPoolSize> ci);
    DescriptorPool(std::initializer_list<std::shared_ptr<DescriptorSet>> target_sets,int set_count = 1);

    ~DescriptorPool();
    // std::shared_ptr<DescriptorSet> CreateDescriptorSet();
private:
};
} // namespace MCRT