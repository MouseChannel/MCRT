#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MoCheng3D {

    DescriptorPool::DescriptorPool(
            std::vector<std::tuple<vk::DescriptorType, uint32_t>> type_size) {
        auto swapchain_size = Get_Context_Singleton()->Get_SwapChain()->Get_Swapchain_Image_size();
        std::vector<vk::DescriptorPoolSize> pool_size(type_size.size());
        for (int i = 0; i < type_size.size(); i++) {
            auto type = std::get<0>(type_size[i]);
            auto size = std::get<1>(type_size[i]);
            pool_size[i].setDescriptorCount(size * 10).setType(type);
        }
        auto ss = pool_size[0];
        pool_size.push_back(ss);
        pool_size.push_back(ss);
        pool_size.emplace_back(vk::DescriptorPoolSize()
                                       .setDescriptorCount(10)
                                       .setType(vk::DescriptorType::eCombinedImageSampler));
        // pool_size.setType(type)
        //     .setDescriptorCount(size);

        vk::DescriptorPoolCreateInfo create_info;
        create_info.setPoolSizes(pool_size)
                .setMaxSets(swapchain_size * 10);
        m_handle = Get_Context_Singleton()
                ->Get_Device()
                ->Get_handle()
                .createDescriptorPool(create_info);
    }

    DescriptorPool::~DescriptorPool() {
        Get_Context_Singleton()
                ->Get_Device()
                ->Get_handle()
                .destroyDescriptorPool(m_handle);
    }

} // namespace MoCheng3D