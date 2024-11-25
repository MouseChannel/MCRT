#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {

DescriptorPool::DescriptorPool(std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_size)
{
    // auto swapchain_size = Get_Context_Singleton()
    //                           ->get_swapchain()
    //                           ->Get_Swapchain_Image_size();
    std::vector<vk::DescriptorPoolSize> pool_size(type_size.size());
    for (int i = 0; i < type_size.size(); i++) {
        auto type = std::get<0>(type_size[i]);
        auto size = std::get<1>(type_size[i]);
        pool_size[i].setDescriptorCount(size).setType(type);
    }
    // pool_size.setType(type)
    //     .setDescriptorCount(size);

    vk::DescriptorPoolCreateInfo create_info;
    create_info.setPoolSizes(pool_size)
        .setMaxSets(100);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createDescriptorPool(create_info);
}
DescriptorPool::DescriptorPool(std::vector<vk::DescriptorPoolSize> ci)
{

    vk::DescriptorPoolCreateInfo create_info;
    create_info.setMaxSets(16)
        .setPoolSizes(ci);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createDescriptorPool(create_info);
}
DescriptorPool::DescriptorPool(std::initializer_list<std::shared_ptr<DescriptorSet>> target_sets,int set_count)
{
    std::map<vk::DescriptorType, uint32_t> type_map;

    for (auto i : target_sets) {
        auto cur_type_map = i->GetTypeMap();
        for (auto [k, v] : cur_type_map) {
            type_map[k] += v*set_count;
        }
    }
    std::vector<vk::DescriptorPoolSize> pool_infos(type_map.size());

    std::transform(type_map.begin(),
                   type_map.end(),
                   pool_infos.begin(),
                   [&](auto item) {
                       return vk::DescriptorPoolSize()
                           .setDescriptorCount(item.second)
                           .setType(item.first);
                   });
    vk::DescriptorPoolCreateInfo create_info;
    create_info.setMaxSets(16)
        .setPoolSizes(pool_infos);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createDescriptorPool(create_info);
}

DescriptorPool::~DescriptorPool()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroyDescriptorPool(m_handle);
}

} // namespace MCRT