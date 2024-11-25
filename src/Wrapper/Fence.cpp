#include "Wrapper/Fence.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {
Fence::Fence()
{
    vk::FenceCreateInfo create_info;
    create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createFence(
                       create_info);
}
Fence::~Fence()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroyFence(m_handle);
}
} // namespace MCRT