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
        .setMaxSets(10);
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

} // namespace MoCheng3D