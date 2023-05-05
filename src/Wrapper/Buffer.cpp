
#include "Wrapper/Buffer.hpp"
#include "Helper/CommandManager.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"

namespace MCRT {
Buffer::Buffer(size_t size,
               vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags property,
               bool permanent)
    : m_usage(usage)
    , permanent(permanent)
    , m_size(size)
{
    CreateBuffer(size, usage);
    memory_info = QueryMemoryInfo(property);
    // vk::MemoryPropertyFlagBits::
    // VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT ;
    // vk::MemoryAllocateFlagBits::eDeviceAddress;
    // vk::MemoryAllocateFlags
    AllocateMemory();
    BindMemory2Buffer();
}
Buffer::~Buffer()
{

    auto& device = Get_Context_Singleton()->get_device()->get_handle();
    device.destroyBuffer(m_handle);
    device.freeMemory(memory);
    // vkDestroyBuffer(device, m_handle, nullptr);
}
void Buffer::CreateBuffer(size_t size, vk::BufferUsageFlags usage)
{
    vk::BufferCreateInfo allocate_info;
    allocate_info.setSize(size)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createBuffer(allocate_info);
}
Buffer::MemoryInfo Buffer::QueryMemoryInfo(vk::MemoryPropertyFlags property)
{
    MemoryInfo info;
    auto requirements = Get_Context_Singleton()
                            ->get_device()
                            ->get_handle()
                            .getBufferMemoryRequirements(m_handle);
    // auto re = requirements.
    info.size = requirements.size;
    auto properties = Get_Context_Singleton()
                          ->get_device()
                          ->Get_Physical_device()
                          .getMemoryProperties();

    for (int i = 0; i < properties.memoryTypeCount; i++) {
        // satisfy both buffer memory_property AND asked memory_property
        if ((1 << i) & requirements.memoryTypeBits &&
            properties.memoryTypes[i].propertyFlags & property) {
            info.index = i;
            break;
        }
    }
    return info;
}
void Buffer::AllocateMemory()
{
    vk::MemoryAllocateInfo allocate_info;
    allocate_info.setMemoryTypeIndex(memory_info.index)
        .setAllocationSize(memory_info.size);
    vk::MemoryAllocateFlagsInfo flag;
    flag.setFlags(vk::MemoryAllocateFlagBits::eDeviceAddress);
    if (m_usage & vk::BufferUsageFlagBits::eShaderDeviceAddress) {
        allocate_info.setPNext(&flag);
    }

    memory = Get_Context_Singleton()
                 ->get_device()
                 ->get_handle()
                 .allocateMemory(allocate_info);
    int a = 0;
}
void Buffer::BindMemory2Buffer()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .bindBufferMemory(m_handle,
                          memory,
                          0);
}
void Buffer::Map(uint32_t offset, uint32_t size)
{
    mapped_data = Get_Context_Singleton()
                      ->get_device()
                      ->get_handle()
                      .mapMemory(memory,
                                 offset,
                                 size);
}
void Buffer::Unmap()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .unmapMemory(memory);
}
void Buffer::Update(void* data, size_t size)
{

    Map(0, size);
    std::memcpy(mapped_data, data, size);
    if (permanent) {
        return;
    }
    Unmap();
}
std::shared_ptr<Buffer> Buffer::CreateDeviceBuffer(void* data,
                                                   size_t size,
                                                   vk::BufferUsageFlags usage)
{
    if (!data) {
        std::shared_ptr<Buffer> res;
        res.reset(new Buffer(size,
                             usage,
                             vk::MemoryPropertyFlagBits::eDeviceLocal,
                             true));
        return res;
    }
    std::shared_ptr<Buffer> host_buffer, device_buffer;
    host_buffer.reset(new Buffer(size,
                                 vk::BufferUsageFlagBits::eTransferSrc,
                                 vk::MemoryPropertyFlagBits::eHostVisible |
                                     vk::MemoryPropertyFlagBits::eHostCoherent));
    if (data)
        host_buffer->Update(data, size);

    device_buffer.reset(new Buffer(size,
                                   vk::BufferUsageFlagBits::eTransferDst | usage,
                                   vk::MemoryPropertyFlagBits::eDeviceLocal,
                                   true));

    auto graphic_queue = Context::Get_Singleton()
                             ->get_device()
                             ->Get_Graphic_queue();

    CommandManager::ExecuteCmd(graphic_queue, [&](auto& cmd_buffer) {
        vk::BufferCopy regin;
        regin.setSize(host_buffer->GetSize())
            .setDstOffset(0)
            .setSrcOffset(0);
        cmd_buffer.copyBuffer(
            host_buffer->get_handle(),
            device_buffer->get_handle(),
            regin);
    });

    return device_buffer;
}
std::shared_ptr<Buffer> Buffer::create_buffer(void* data, size_t size, vk::BufferUsageFlags usage)
{
    std::shared_ptr<Buffer> res;
    res.reset(new Buffer(size,
                         usage,
                         vk::MemoryPropertyFlagBits::eHostVisible,
                         true));
    if (data)
        res->Update(data, size);
    return res;
}
vk::DeviceAddress Buffer::get_address()
{
    if (m_buffer_address == 0) {

        vk::BufferDeviceAddressInfo address_info;
        address_info.setBuffer(m_handle);
        m_buffer_address = Get_Context_Singleton()
                               ->get_device()
                               ->get_handle()
                               .getBufferAddress(
                                   address_info);
    }
    return m_buffer_address;
}

} // namespace MCRT