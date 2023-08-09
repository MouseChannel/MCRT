#include "Helper/DescriptorManager.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include <map>
#include <tuple>

namespace MCRT {
void Descriptor_Manager::CreateDescriptorPool(Which_Set which_set)
{

    std::map<vk::DescriptorType, uint32_t> type_map;
    auto& descriptor_set = descriptorsets[which_set];

    for (auto& [index, i] : descriptor_set->get_buffer_data()) {

        auto type = i.binding.descriptorType;
        type_map[type]++;
    }

   
    for (auto& [index, i] : descriptor_set->_get_image_data()) {

        auto type = i.binding.descriptorType;
        type_map[type]++;
    }

   
    for (auto& [index, i] : descriptor_set->get_as_data()) {

        auto type = i.binding.descriptorType;
        type_map[type]++;
    }
    std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_arr;
    for (auto& i : type_map) {
        type_arr.emplace_back(std::tuple<vk::DescriptorType, uint32_t> { i.first, 10 });
    }
    descriptorPools[which_set].reset(new DescriptorPool(type_arr));
}
void Descriptor_Manager::update_descriptor_set(Which_Set which_set)
{
    auto& cur_set = descriptorsets[which_set];
    if (cur_set->get_handle().empty() || !cur_set->get_handle()[0])
        cur_set->create(descriptorPools[which_set]);
    descriptorsets[which_set]->Update();
}

vk::DescriptorSetLayout Descriptor_Manager::Get_DescriptorSet_layout(Which_Set which_set)
{

    return descriptorsets[which_set]->get_layout();
}

Descriptor_Manager::~Descriptor_Manager()
{
    for (auto& descriptorPool : descriptorPools)
        descriptorPool.reset();
}
}