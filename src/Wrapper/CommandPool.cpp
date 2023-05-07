#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"
namespace MCRT {
CommandPool::CommandPool()
{
    vk::CommandPoolCreateInfo create_info;
    create_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    m_handle =
        Get_Context_Singleton()->get_device()->get_handle().createCommandPool(
            create_info);
    
}
CommandPool::~CommandPool()
{
    Get_Context_Singleton()->get_device()->get_handle().destroyCommandPool(
        m_handle);
}
} // namespace MCRT