#include "Helper/CUDA/CUDAManager.hpp"
#include "Wrapper/Device.hpp"

#include <Wrapper/Buffer.hpp>
#include <Wrapper/SwapChain.hpp>


namespace MCRT {
void CUDA_Manager::Init()
{
    // cudaDeviceProp deviceProp;
    // int device_count = 0;
    // cudaGetDeviceCount(&device_count);
    // cudaGetDeviceProperties(&deviceProp, 0);
    // int ret = memcmp((void*)&deviceProp.uuid, Context::Get_Singleton()->get_device()->get_deviceUUID(), VK_UUID_SIZE);

    // if (ret == 0) {
    //     checkCudaErrors(cudaSetDevice(current_device));
    //     checkCudaErrors(cudaGetDeviceProperties(&deviceProp, current_device));
    //     printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n",
    //            current_device, deviceProp.name, deviceProp.major,
    //            deviceProp.minor);
    //
    //     return current_device;
    // }
    int aut = 0;
}

__global__ void real_run(int* device_data)
{
    const float freq = 4.0f;
    const size_t stride = gridDim.x * blockDim.x;
    for (int i = 0; i < blockDim.x; i++) {

        device_data[i] = 1234;
    }
    printf("%d %d \n", gridDim.x, blockDim.y);
    // Iterate through the entire array in a way that is
    // independent of the grid configuration
    // for (size_t tid = blockIdx.x * blockDim.x + threadIdx.x; tid < width * height;
    //      tid += stride) {
    //     // Calculate the x, y coordinates
    //     const size_t y = tid / width;
    //     const size_t x = tid - y * width;
    //     // Normalize x, y to [0,1]
    //     const float u = ((2.0f * x) / width) - 1.0f;
    //     const float v = ((2.0f * y) / height) - 1.0f;
    //     // Calculate the new height value
    //     const float w = 0.5f * sinf(u * freq + time) * cosf(v * freq + time);
    //     // Store this new height value
    //     heightMap[tid] = w;
    //      }
}

void CUDA_Manager::Run(void* device_data)
{
    real_run<<<1,10>>>((int*)device_data);
}

void CUDA_Manager::CreateVKExternalSamephore()
{

    waitSemaphore.reset(new Semaphore(true));
    signalSemaphore.reset(new Semaphore(true));

}


void CUDA_Manager::ImportCudaExternalSemaphore()
{
    cudaExternalSemaphoreHandleDesc externalSemaphoreHandleDesc = {};
    externalSemaphoreHandleDesc.type =
        cudaExternalSemaphoreHandleTypeOpaqueFd;
    externalSemaphoreHandleDesc.handle.fd =
        (int)(uintptr_t)GetSemaphoreHandle(waitSemaphore);
    auto res = cudaImportExternalSemaphore(&m_cudaWaitSemaphore, &externalSemaphoreHandleDesc);
    if (res != cudaSuccess) {
        throw std::runtime_error("fail ImportCudaExternalSemaphore");
    };
    //////////////////////////
    externalSemaphoreHandleDesc.handle.fd =
        (int)(uintptr_t)GetSemaphoreHandle(signalSemaphore);
    res = cudaImportExternalSemaphore(&m_cudaSignalSemaphore, &externalSemaphoreHandleDesc);
    if (res != cudaSuccess) {
        throw std::runtime_error("fail ImportCudaExternalSemaphore");
    };
}

void* CUDA_Manager::GetSemaphoreHandle(std::shared_ptr<Semaphore>& semaphore)
{
    // int fd;
    vk::SemaphoreGetFdInfoKHR semaphore_get_fd_info;
    semaphore_get_fd_info.setHandleType(vk::ExternalSemaphoreHandleTypeFlagBits::eOpaqueFd)
                         .setSemaphore(semaphore->get_handle());
    auto m_device = Context::Get_Singleton()->get_device()->get_handle();

    int fd = m_device.getSemaphoreFdKHR(semaphore_get_fd_info);
    return (void*)(uintptr_t)fd;
}

void* CUDA_Manager::GetMemHandle(std::shared_ptr<Buffer> buffer)
{
    auto memoryGetFdInfoKHR = vk::MemoryGetFdInfoKHR()
                              .setMemory(buffer->GetMemory())
                              .setHandleType(vk::ExternalMemoryHandleTypeFlagBitsKHR::eOpaqueFd);
    auto fd = Context::Get_Singleton()->get_device()->get_handle().getMemoryFdKHR(memoryGetFdInfoKHR);
    return (void*)(uintptr_t)fd;

}

void CUDA_Manager::ImportCudaExternalMemory(void** cudaPtr, cudaExternalMemory_t& cudaMem, const std::shared_ptr<Buffer>& buffer)
{
    cudaExternalMemoryHandleDesc externalMemoryHandleDesc = {};
    externalMemoryHandleDesc.type = cudaExternalMemoryHandleTypeOpaqueFd;
    externalMemoryHandleDesc.size = buffer->GetSize();
    externalMemoryHandleDesc.handle.fd =
        (int)(uintptr_t)GetMemHandle(buffer);
    if (cudaImportExternalMemory(&cudaMem,
                                 &externalMemoryHandleDesc) != cudaSuccess) {
        throw std::runtime_error("err cudaImportExternalMemory");
    }
    cudaExternalMemoryBufferDesc externalMemBufferDesc = {};
    externalMemBufferDesc.offset = 0;
    externalMemBufferDesc.size = buffer->GetSize();
    externalMemBufferDesc.flags = 0;
    if (cudaExternalMemoryGetMappedBuffer(cudaPtr,
                                          cudaMem,
                                          &externalMemBufferDesc) != cudaSuccess) {
        throw std::runtime_error("err cudaExternalMemoryGetMappedBuffer");
    }

}


}
