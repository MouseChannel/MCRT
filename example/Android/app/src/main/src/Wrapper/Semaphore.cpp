#include "Wrapper/Semaphore.hpp"
#include "Wrapper/Device.hpp"
namespace MoCheng3D {
Semaphore::Semaphore()
{
    vk::SemaphoreCreateInfo create_info;
    m_handle = Get_Context_Singleton()
                   ->Get_Device()
                   ->Get_handle()
                   .createSemaphore(
                       create_info);
}
Semaphore::~Semaphore()
{
    Get_Context_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroySemaphore(m_handle);
}
} // namespace MoCheng3D