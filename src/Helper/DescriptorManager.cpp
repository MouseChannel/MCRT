#include "Helper/DescriptorManager.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include <map>
#include <tuple>

namespace MCRT {

void Descriptor_Manager::Make_DescriptorSet(std::shared_ptr<Buffer> data,
                                            uint32_t binding_index,
                                            vk::DescriptorType type,
                                            vk::ShaderStageFlags shader_stage)
{
    vk::DescriptorSetLayoutBinding binding;
    binding.setBinding(binding_index)
        .setDescriptorType(type)
        .setStageFlags(shader_stage)
        .setDescriptorCount(1);

    descriptorSet_buffer_binding_map.emplace_back(
        DescriptorSet_Buffer_Binding { data, binding });
}

void Descriptor_Manager::Make_DescriptorSet(std::shared_ptr<Image> data,
                                            uint32_t binding_index,
                                            vk::DescriptorType type,
                                            vk::ShaderStageFlags shader_stage)
{
    vk::DescriptorSetLayoutBinding binding;
    binding.setBinding(binding_index)
        .setDescriptorType(type)
        .setStageFlags(shader_stage)
        .setDescriptorCount(1);

    descriptorSet_image_binding_map.emplace_back(
        DescriptorSet_Image_Binding { data, binding });
}
void Descriptor_Manager::Make_DescriptorSet(vk::AccelerationStructureKHR as_data, uint32_t binding_index, vk::DescriptorType type, vk::ShaderStageFlags shader_stage)
{
    vk::DescriptorSetLayoutBinding binding;
    binding.setBinding(binding_index)
        .setDescriptorCount(1)
        .setStageFlags(shader_stage)
        .setDescriptorType(type);
    descriptorSet_rt_binding_map.emplace_back(DescriptorSet_RT_Binding { .as_data { as_data }, .binding { binding } });
    // binding.s
}
void Descriptor_Manager::CreateDescriptorPool()
{
    assert(!descriptorSet_buffer_binding_map.empty());
    std::map<vk::DescriptorType, uint32_t> type_map;
    for (auto& i : descriptorSet_buffer_binding_map) {

        auto type = i.binding.descriptorType;
        type_map[type]++;
    }
    std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_arr;
    for (auto& i : type_map) {
        type_arr.emplace_back(std::tuple<vk::DescriptorType, uint32_t> { i.first, 10 });
    }
    descriptorPool.reset(new DescriptorPool(type_arr));
}
std::vector<vk::DescriptorSetLayoutBinding>& Descriptor_Manager::Get_layout_bindings()
{
    assert(!descriptorSet_buffer_binding_map.empty());
    if (layout_bindings.empty()) {
        for (auto& i : descriptorSet_buffer_binding_map) {
            layout_bindings.emplace_back(i.binding);
        }
        for (auto& i : descriptorSet_image_binding_map) {
            layout_bindings.emplace_back(i.binding);
        }
    }
    return layout_bindings;
}
vk::DescriptorSetLayout& Descriptor_Manager::Get_DescriptorSet_layout()
{
    if (!descriptor_layout) {
        vk::DescriptorSetLayoutCreateInfo layout_create_info;
        layout_create_info.setBindings(Get_layout_bindings());
        descriptor_layout = Context::Get_Singleton()
                                ->get_device()
                                ->Get_handle()
                                .createDescriptorSetLayout(layout_create_info);
    }
    return descriptor_layout;
}
void Descriptor_Manager::CreateUpdateDescriptorSet()
{
    descriptorSet.reset(new DescriptorSet(descriptorPool, Get_DescriptorSet_layout()));
    for (auto& i : descriptorSet_buffer_binding_map) {
        auto binding = i.binding;
        auto buffer = i.buffer;
        descriptorSet->Update(buffer,
                              binding.binding,
                              binding.descriptorType);
    }
    for (auto& i : descriptorSet_image_binding_map) {
        auto binding = i.binding;
        auto image = i.image;
        descriptorSet->Update(image,
                              binding.binding,
                              binding.descriptorType);
    }
    for (auto& i : descriptorSet_buffer_binding_map) { }
}

Descriptor_Manager::~Descriptor_Manager()
{
    layout_bindings.clear();
    descriptorSet.reset();
    auto& de = Context::Get_Singleton();
    Context::Get_Singleton()
        ->get_device()
        ->Get_handle()
        .destroyDescriptorSetLayout(Get_DescriptorSet_layout());

    descriptorSet_buffer_binding_map.clear();
    descriptorSet_image_binding_map.clear();
    descriptorPool.reset();
}
}