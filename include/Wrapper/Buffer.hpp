
#pragma once

#include "Wrapper/Component.hpp"

namespace MCRT {
class CommandBuffer;
class Buffer : public Component<vk::Buffer, Buffer> {
public:
    Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property, bool permanent = false);
    ~Buffer();
    void Update(void* data, size_t size);
    void Update(std::vector<void*> data, std::vector<size_t> size);
    [[nodiscard("Missing Size")]] auto GetSize()
    {
        return m_size;
    }
    [[nodiscard]] vk::DeviceAddress get_address();
    static std::shared_ptr<Buffer> CreateDeviceBuffer(void* data, size_t size, vk::BufferUsageFlags usage);
    static std::shared_ptr<Buffer> create_buffer(void* data, size_t size, vk::BufferUsageFlags usage);
    template <typename T>
    static std::shared_ptr<Buffer> create_buffer(std::vector<T> data_, vk::BufferUsageFlags usage)
    {
        return create_buffer(data_.data(), sizeof(T) * data_.size(), usage);
    }

private:
    struct MemoryInfo {
        size_t size;
        uint32_t index;
    } memory_info;
    vk::DeviceMemory memory;
    size_t m_size;
    vk::BufferUsageFlags m_usage;
    bool permanent { false };
    void* mapped_data { nullptr };
    vk::DeviceAddress m_buffer_address { 0 };
    void Map(uint32_t offset, uint32_t size);
    void Unmap();
    void CreateBuffer(size_t size, vk::BufferUsageFlags usage);
    MemoryInfo QueryMemoryInfo(vk::MemoryPropertyFlags property);
    void AllocateMemory();
    void BindMemory2Buffer();
};
} // namespace MCRT