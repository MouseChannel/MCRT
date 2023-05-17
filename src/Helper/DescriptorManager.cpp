#include "Helper/DescriptorManager.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include <map>
#include <tuple>

namespace MCRT {
void Descriptor_Manager::add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<Buffer> data, vk::DescriptorSetLayoutBinding binding)
{
    // descriptor_set_buffer_binding.push_back(Descriptor_Set_Binding {
    //     data,
    //     binding });
    descriptor_set_binding[which_set].push_back({ data, binding, e_buffer });
}
void Descriptor_Manager::add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<Image> data, vk::DescriptorSetLayoutBinding binding)
{
    // descriptor_set_image_binding.push_back(Descriptor_Set_Binding {
    //     data,
    //     binding });

    descriptor_set_binding[e_image].push_back({ data, binding, e_image });
}
void Descriptor_Manager::add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<AccelerationStructure> data, vk::DescriptorSetLayoutBinding binding)
{
    // descriptor_set_tlas_binding.push_back(Descriptor_Set_Binding {
    //     data,
    //     binding });

    descriptor_set_binding[e_tlas].push_back({ data, binding, e_tlas });
}
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

void Descriptor_Manager::CreateUpdateDescriptorSet(Which_Set which_set)
{
    descriptorSets[which_set].reset(new DescriptorSet(descriptorPools[which_set], Get_DescriptorSet_layout(which_set)));
    for (auto& i : descriptor_set_binding[which_set]) {
        auto binding = i.binding;

        auto& data = i.data;
        auto& data_type = data.type();
        // auto d = decltype(data);

        // auto type_id = typeid(std::shared_ptr<AccelerationStructure>();
        if (data_type == typeid(std::shared_ptr<Image>)) {
            descriptorSets[which_set]->Update(std::any_cast<std::shared_ptr<Image>>(data), binding.binding, binding.descriptorType);
        } else if (data_type == typeid(std::shared_ptr<Buffer>)) {
            descriptorSets[which_set]->Update(std::any_cast<std::shared_ptr<Buffer>>(data), binding.binding, binding.descriptorType);
        } else if (data_type == typeid(std::shared_ptr<AccelerationStructure_Top>)) {
            descriptorSets[which_set]->Update(std::any_cast<std::shared_ptr<AccelerationStructure_Top>>(data), binding.binding, binding.descriptorType);
        }
    }
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