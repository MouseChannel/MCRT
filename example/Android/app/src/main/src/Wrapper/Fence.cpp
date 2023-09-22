#include "Wrapper/Fence.hpp"
#include "Wrapper/Device.hpp"

namespace MoCheng3D {
Fence::Fence()
{
    vk::FenceCreateInfo create_info;

    m_handle = Get_Context_Singleton()
                   ->Get_Device()
                   ->Get_handle()
                   .createFence(
                       create_info);
}
Fence::~Fence()
{
    Get_Context_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroyFence(m_handle);
}
} // namespace MoCheng3D