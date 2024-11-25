#include "Wrapper/CommandPool.hpp"
#include "Wrapper/Device.hpp"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
namespace MoCheng3D {

CommandPool::CommandPool()
{

    vk::CommandPoolCreateInfo create_info;
    create_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    m_handle = Get_Context_Singleton()
                   ->Get_Device()
                   ->Get_handle()
                   .createCommandPool(
                       create_info);
}
CommandPool::~CommandPool()
{
    Context::Get_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroyCommandPool(m_handle);
}

} // namespace MoCheng3D