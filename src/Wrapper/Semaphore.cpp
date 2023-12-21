#include "Wrapper/Semaphore.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {


Semaphore::Semaphore(bool need_export)
{

    vk::SemaphoreCreateInfo create_info;

    //for export to cuda
    vk::ExportSemaphoreCreateInfoKHR export_semaphore_create_info;
    if (need_export) {
        export_semaphore_create_info = vk::ExportSemaphoreCreateInfoKHR()
            .setHandleTypes(vk::ExternalSemaphoreHandleTypeFlagBits::eOpaqueFd);
        create_info.setPNext(&export_semaphore_create_info);
    }

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
