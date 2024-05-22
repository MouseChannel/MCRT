#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include <map>

namespace MCRT {
DescriptorSet::DescriptorSet()
{
}

void DescriptorSet::build(std::shared_ptr<DescriptorPool> pool, int size)
{
    vk::DescriptorSetAllocateInfo allocate_info;
    // auto swapchain_size = Get_Context_Singleton()->get_swapchain()->Get_Swapchain_Image_size();
    std::vector<vk::DescriptorSetLayout> layouts(size);
    for (int i = 0; i < layouts.size(); i++) {
        layouts[i] = get_layout();
    }
    allocate_info.setDescriptorPool(pool->get_handle())
        .setSetLayouts(layouts)
        // TODO swapchain_size
        .setDescriptorSetCount(size);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .allocateDescriptorSets(allocate_info);
}
DescriptorSet::~DescriptorSet()
{

    Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .destroyDescriptorSetLayout(layout);
}

void DescriptorSet::Add(std::shared_ptr<DescriptorSetTargetBase> descriptorSetTarget)
{
    m_layout_binding[descriptorSetTarget->get_binding_index()] =
        vk::DescriptorSetLayoutBinding()
            .setBinding(descriptorSetTarget->get_binding_index())
            .setDescriptorCount(descriptorSetTarget->get_descriptorSet_count())
            .setStageFlags(descriptorSetTarget->get_shader_stage())
            .setDescriptorType(descriptorSetTarget->get_type());
}
 

vk::DescriptorSetLayout& DescriptorSet::get_layout()
{
  

    if (check_dirty()) {

        layout_bindings.resize(m_layout_binding.size());
        std::transform(m_layout_binding.begin(),
                       m_layout_binding.end(),
                       layout_bindings.begin(),
                       [](auto& item) {
                           return item.second;
                       });
        if (layout != VK_NULL_HANDLE) {
            Context::Get_Singleton()
                ->get_device()
                ->get_handle()
                .destroyDescriptorSetLayout(layout);
        }
        layout = Context::Get_Singleton()
                     ->get_device()
                     ->get_handle()
                     .createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo().setBindings(layout_bindings));
    }
    // }
    return layout;
}
bool DescriptorSet::check_dirty()
{
    if (layout_bindings.empty()) {
        return true;
    }
    if (layout_bindings.size() != m_layout_binding.size()) {
        return true;
    }
    for (int i = 0; i < layout_bindings.size(); i++) {
        auto& old = layout_bindings[i];
        auto& cur = m_layout_binding[old.binding];
        if (old != cur) {
            return true;
        }
    }
    return false;
}
}  