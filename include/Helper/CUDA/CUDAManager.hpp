#include "Helper/Instance_base.hpp"
#include "cuda.h"
#include"cuda_runtime.h"
#include <memory>
#include <Wrapper/Semaphore.hpp>

namespace MCRT {
class Buffer;

class CUDA_Manager : public Instance_base<CUDA_Manager> {
public:
    void Init();
    void Run(void * device_deta);
    /*create vk waitSemaphore signalSemaphore */
    void CreateVKExternalSamephore();
    /* **Link**  cuda waitSemaphore signalSemaphore to vk semaphore*/
    void ImportCudaExternalSemaphore();

    void CreateExternalBuffer();
    void ImportCudaExternalMemory(void** cudaPtr, cudaExternalMemory_t& cudaMem, const std::shared_ptr<Buffer>& buffer);

private:
    void* GetSemaphoreHandle(std::shared_ptr<Semaphore>& semaphore);
    void* GetMemHandle(std::shared_ptr<Buffer> buffer);
    std::shared_ptr<Semaphore>
        // timeline_semaphore,
        waitSemaphore, signalSemaphore;
    cudaExternalSemaphore_t m_cudaWaitSemaphore, m_cudaSignalSemaphore;
    // m_cudaTimelineSemaphore;
};
}
