#include "Helper/DescriptorManager.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include <map>
#include <tuple>

namespace MCRT {

// void Descriptor_Manager::Make_DescriptorSet(std::shared_ptr<Buffer> data,
//                                             uint32_t binding_index,
//                                             vk::DescriptorType type,
//                                             vk::ShaderStageFlags shader_stage)
// {
//     vk::DescriptorSetLayoutBinding binding;
//     binding.setBinding(binding_index)
//         .setDescriptorType(type)
//         .setStageFlags(shader_stage)
//         .setDescriptorCount(1);

//     descriptorSet_buffer_binding_map.emplace_back(
//         DescriptorSet_Buffer_Binding { data, binding });
// }

// void Descriptor_Manager::Make_DescriptorSet(std::shared_ptr<Image> data,
//                                             uint32_t binding_index,
//                                             vk::DescriptorType type,
//                                             vk::ShaderStageFlags shader_stage)
// {
//     vk::DescriptorSetLayoutBinding binding;
//     binding.setBinding(binding_index)
//         .setDescriptorType(type)
//         .setStageFlags(shader_stage)
//         .setDescriptorCount(1);

//     descriptorSet_image_binding_map.emplace_back(
//         DescriptorSet_Image_Binding { data, binding });
// }
// void Descriptor_Manager::Make_DescriptorSet(std::shared_ptr<AccelerationStructure> as_data, uint32_t binding_index, vk::DescriptorType type, vk::ShaderStageFlags shader_stage)
// {
//     vk::DescriptorSetLayoutBinding binding;
//     binding.setBinding(binding_index)
//         .setDescriptorCount(1)
//         .setStageFlags(shader_stage)
//         .setDescriptorType(type);
//     DescriptorSet_RT_Binding res;
//     res.data = as_data;
//     res.binding = binding;
//     descriptorSet_rt_binding_map.emplace_back(res);
//     // binding.s
// }
void Descriptor_Manager::CreateDescriptorPool(Which_Set which_set)
{
    std::map<vk::DescriptorType, uint32_t> type_map;

    for (auto& i : descriptor_set_binding[which_set]) {

        auto type = i.binding.descriptorType;
        type_map[type]++;
    }
    std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_arr;
    for (auto& i : type_map) {
        type_arr.emplace_back(std::tuple<vk::DescriptorType, uint32_t> { i.first, 10 });
    }
    descriptorPools[which_set].reset(new DescriptorPool(type_arr));
}
// std::vector<vk::DescriptorSetLayoutBinding>& Descriptor_Manager::Get_layout_bindings()
// {
//     // assert(!descriptorSet_buffer_binding_map.empty());
//     if (graphic_layout_bindings.empty()) {
//         for (auto& i : descriptor_graphic_set_binding) {
//             graphic_layout_bindings.emplace_back(i.binding);
//         }
//         // for (auto& i : descriptorSet_buffer_binding_map) {
//         //     layout_bindings.emplace_back(i.binding);
//         // }
//         // for (auto& i : descriptorSet_image_binding_map) {
//         //     layout_bindings.emplace_back(i.binding);
//         // }
//         // for (auto& i : descriptorSet_rt_binding_map) {
//         //     layout_bindings.emplace_back(i.binding);
//         // }
//     }
//     return graphic_layout_bindings;
// }
std::vector<vk::DescriptorSetLayoutBinding>& Descriptor_Manager::Get_layout_bindings(Which_Set which_set)
{
    // assert(!descriptorSet_buffer_binding_map.empty());
    if (layout_bindings[which_set].empty()) {
        for (auto& i : descriptor_set_binding[which_set]) {
            layout_bindings[which_set].emplace_back(i.binding);
        }
    }
    return layout_bindings[which_set];
}
// std::vector<vk::DescriptorSetLayoutBinding>& Descriptor_Manager::Get_RT_layout_bindings()
// {
//     // assert(!descriptorSet_buffer_binding_map.empty());
//     if (rt_layout_bindings.empty()) {
//         for (auto& i : rt_descriptor_set_binding) {
//             rt_layout_bindings.emplace_back(i.binding);
//         }
//     }
//     return rt_layout_bindings;
// }
// vk::DescriptorSetLayout& Descriptor_Manager::Get_DescriptorSet_layout()
// {
//     if (!graphic_descriptor_layout) {
//         vk::DescriptorSetLayoutCreateInfo layout_create_info;
//         layout_create_info.setBindings(Get_layout_bindings());
//         graphic_descriptor_layout = Context::Get_Singleton()
//                                         ->get_device()
//                                         ->get_handle()
//                                         .createDescriptorSetLayout(layout_create_info);
//     }
//     return graphic_descriptor_layout;
// }
vk::DescriptorSetLayout& Descriptor_Manager::Get_DescriptorSet_layout(Which_Set which_set)
{
    if (!descriptor_set_layouts[which_set]) {
        vk::DescriptorSetLayoutCreateInfo layout_create_info;
        layout_create_info.setBindings(Get_layout_bindings(which_set));
        descriptor_set_layouts[which_set] = Context::Get_Singleton()
                                                ->get_device()
                                                ->get_handle()
                                                .createDescriptorSetLayout(layout_create_info);
    }
    return descriptor_set_layouts[which_set];
}

// vk::DescriptorSetLayout Descriptor_Manager::get_RT_DescriptorSet_layout()
// {
//     if (!descriptor_RT_layout) {
//         vk::DescriptorSetLayoutCreateInfo layout_create_info;
//         layout_create_info.setBindings(Get_layout_bindings());
//         descriptor_RT_layout = Context::Get_Singleton()
//                                    ->get_device()
//                                    ->get_handle()
//                                    .createDescriptorSetLayout(layout_create_info);
//     }
//     return descriptor_RT_layout;
// }

void Descriptor_Manager::CreateUpdateDescriptorSet(Which_Set which_set)
{
    descriptorSets[which_set].reset(new DescriptorSet(descriptorPools[which_set], Get_DescriptorSet_layout(which_set)));
    for (auto& i : descriptor_set_binding[which_set]) {
        auto binding = i.binding;

        descriptorSets[which_set]->Update(i.data, binding.binding, binding.descriptorType);
    }
    // for (auto& i : descriptorSet_buffer_binding_map) {
    //     auto binding = i.binding;
    //     auto buffer = i.data;
    //     descriptorSet->Update(buffer,
    //                           binding.binding,
    //                           binding.descriptorType);
    // }
    // for (auto& i : descriptorSet_image_binding_map) {
    //     auto binding = i.binding;
    //     auto image = i.data;
    //     descriptorSet->Update(image,
    //                           binding.binding,
    //                           binding.descriptorType);
    // }
    // for (auto& i : descriptorSet_buffer_binding_map) { }
}

Descriptor_Manager::~Descriptor_Manager()
{
    layout_bindings.clear();
    for (auto& descriptorSet : descriptorSets)
        descriptorSet.reset();
    auto& de = Context::Get_Singleton();
    for (auto& i : descriptor_set_layouts)
        Context::Get_Singleton()
            ->get_device()
            ->get_handle()
            .destroyDescriptorSetLayout(i);

    // descriptorSet_buffer_binding_map.clear();
    // descriptorSet_image_binding_map.clear();
    for (auto& descriptorPool : descriptorPools)
        descriptorPool.reset();
}
}