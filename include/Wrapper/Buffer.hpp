#pragma once

#include "Wrapper/Component.hpp"

// #define VK_USE_CUDA
namespace MCRT {
class CommandBuffer;

class Buffer : public Component<vk::Buffer, Buffer> {
public:
    Buffer() = default;
    Buffer(size_t size,
           vk::BufferUsageFlags usage,
           vk::MemoryPropertyFlags property,
           bool permanent = false,
           bool need_export = false);
    // Buffer(size_t size,vk::BufferUsageFlags usage,
    //           vk::MemoryPropertyFlags property,vk::ExternalSemaphoreHandleTypeFlags extMemHandleType);
    ~Buffer();
    void Update(void* data, size_t size);
    void Update(std::vector<void*> data, std::vector<size_t> size);

    std::vector<uint8_t> Get_mapped_data(uint32_t offset = 0);

    [[nodiscard("Missing Size")]] auto GetSize()
    {
        return m_size;
    }

    [[nodiscard("Missing memory")]] auto GetMemory()
    {
        return memory;
    }

    [[nodiscard]] vk::DeviceAddress get_address();
    static void CopyBuffer(std::shared_ptr<Buffer> src, std::shared_ptr<Buffer> dst);
    static std::shared_ptr<Buffer> CreateDeviceBuffer(void* data, size_t size, vk::BufferUsageFlags usage);
    static std::shared_ptr<Buffer> create_buffer(void* data, size_t size, vk::BufferUsageFlags usage);
    static std::shared_ptr<Buffer> CreateExternalBuffer(void* data, size_t size, vk::BufferUsageFlags usage_flags, vk::MemoryPropertyFlags properties, vk::ExternalMemoryHandleTypeFlagsKHR extMemHandleType);


    template <typename T>
    static std::shared_ptr<Buffer> create_buffer(std::vector<T> data_, vk::BufferUsageFlags usage)
    {
        return create_buffer(data_.data(), sizeof(T) * data_.size(), usage);
    }

    template <typename T>
    std::vector<T> GetData()
    {
        auto length = m_size / sizeof(T);
        std::vector<T> res(length);

        auto data = Map(0, 0);
        memcpy(res.data(), data, m_size);
        return res;

    }

private:
    struct MemoryInfo {
        size_t size;
        uint32_t index;
    } memory_info{};

    vk::DeviceMemory memory;
    size_t m_size;
    vk::BufferUsageFlags m_usage;
    bool m_permanent{ false };
    bool m_need_export{ false };
    void* mapped_data{ nullptr };
    vk::DeviceAddress m_buffer_address{ 0 };


    vk::ExternalMemoryHandleTypeFlagsKHR m_extMemHandleType{ vk::ExternalMemoryHandleTypeFlagBitsKHR::eOpaqueFd };

    void BufferInit(size_t size,
                    vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags property,
                    bool permanent);
    void* Map(uint32_t offset, uint32_t size);
    void Unmap();
    void CreateBuffer(size_t size, vk::BufferUsageFlags usage);
    MemoryInfo QueryMemoryInfo(vk::MemoryPropertyFlags property);
    void AllocateMemory();
    void BindMemory2Buffer();
};
} // namespace MCRT
