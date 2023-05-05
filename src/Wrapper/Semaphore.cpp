#include "Wrapper/Semaphore.hpp"
#include "Wrapper/Device.hpp"
namespace MCRT {
Semaphore::Semaphore()
{
    vk::SemaphoreCreateInfo create_info;
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createSemaphore(
                       create_info);
}
Semaphore::~Semaphore()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroySemaphore(m_handle);
}
} // namespace MoCheng3D