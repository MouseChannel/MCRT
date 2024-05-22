#include "Wrapper/Buffer.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/Debugger.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Command_Pool.hpp"
#include "Wrapper/Device.hpp"

// #include <ranges>
//  #define VK_USE_CUDA

namespace MCRT {
Buffer::Buffer(size_t size,
               vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags property,
               bool permanent,
               bool need_export)
    : m_usage(usage)
    , m_permanent(permanent)
    , m_size(size)
    , m_need_export(need_export)

{
    CreateBuffer(size, usage);
    memory_info = QueryMemoryInfo(property);

    AllocateMemory();
    BindMemory2Buffer();
    // BufferInit(size, usage, property, permanent);
    // std::cout << "create buffer:" << m_handle << std::endl;
}

void Buffer::BufferInit(size_t size,
                        vk::BufferUsageFlags usage,
                        vk::MemoryPropertyFlags property,
                        bool permanent)
{
    m_usage = usage;
    m_size = size;
    m_permanent = permanent;
    CreateBuffer(size, usage);
    memory_info = QueryMemoryInfo(property);

    AllocateMemory();
    BindMemory2Buffer();
}

// Buffer::Buffer(size_t size,
//                vk::BufferUsageFlags usage,
//                vk::MemoryPropertyFlags property,
//                vk::ExternalMemoryHandleTypeFlagsKHR extMemHandleType)
// {
// #if defined(VK_USE_CUDA)
//
//     auto externalMemoryBufferInfo = vk::ExternalMemoryBufferCreateInfo()
//         .setHandleTypes(extMemHandleType);
// #endif
//
//     vk::BufferCreateInfo allocate_info;
//     allocate_info.setSize(size)
//                  .setUsage(usage)
//                  .setSharingMode(vk::SharingMode::eExclusive)
//                  .setPNext(&externalMemoryBufferInfo);
//     m_handle = Get_Context_Singleton()
//                ->get_device()
//                ->get_handle()
//                .createBuffer(allocate_info);
//     memory_info = QueryMemoryInfo(property);
//     //allocate memory
//     auto vulkanExportMemoryAllocateInfoKHR = vk::ExportMemoryAllocateInfoKHR()
//         .setHandleTypes(extMemHandleType);
//
// }

Buffer::~Buffer()
{

    // std::cout<<"delete buffer:"<<  m_handle << std::endl;
    auto& device = Get_Context_Singleton()->get_device()->get_handle();
    device.destroyBuffer(m_handle);
    device.freeMemory(memory);
    // vkDestroyBuffer(device, m_handle, nullptr);
}

void Buffer::CreateBuffer(size_t size, vk::BufferUsageFlags usage)
{
    vk::BufferCreateInfo allocate_info;
    vk::ExternalMemoryBufferCreateInfo externalMemoryBufferInfo;
    if (m_need_export) {
        externalMemoryBufferInfo = vk::ExternalMemoryBufferCreateInfo()
                                       .setHandleTypes(m_extMemHandleType);
        allocate_info.setSize(size)
            .setUsage(usage)
            .setPNext(&externalMemoryBufferInfo)
            .setSharingMode(vk::SharingMode::eExclusive);
    } else {

        allocate_info.setSize(size)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive);
    }
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
    vk::ExportMemoryAllocateInfoKHR vulkanExportMemoryAllocateInfoKHR;
    if (m_need_export) {
        vulkanExportMemoryAllocateInfoKHR = vk::ExportMemoryAllocateInfoKHR()
                                                .setHandleTypes(m_extMemHandleType);

        allocate_info.setMemoryTypeIndex(memory_info.index)
            .setPNext(&vulkanExportMemoryAllocateInfoKHR)
            .setAllocationSize(memory_info.size);
    } else {

        allocate_info.setMemoryTypeIndex(memory_info.index)
            .setAllocationSize(memory_info.size);
    }
    // #if defined(VK_USE_CUDA)
    //
    //     auto vulkanExportMemoryAllocateInfoKHR = vk::ExportMemoryAllocateInfoKHR()
    //         .setHandleTypes(m_extMemHandleType);
    // #endif
    //     vk::MemoryAllocateInfo allocate_info;
    //     allocate_info.setMemoryTypeIndex(memory_info.index)
    // #if defined(VK_USE_CUDA)
    //         .setPNext(&vulkanExportMemoryAllocateInfoKHR)
    // #endif
    //
    //         .setAllocationSize(memory_info.size);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
    vk::MemoryAllocateFlagsInfo flag;
    flag.setFlags(vk::MemoryAllocateFlagBits::eDeviceAddress);
    if (m_usage & vk::BufferUsageFlagBits::eShaderDeviceAddress) {
        if (m_need_export) {

            vulkanExportMemoryAllocateInfoKHR.setPNext(&flag);
        } else {
            allocate_info.setPNext(&flag);
        }
    }
#endif

    memory = Get_Context_Singleton()
                 ->get_device()
                 ->get_handle()
                 .allocateMemory(allocate_info);
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

void* Buffer::Map(uint32_t offset, uint32_t size)
{
    mapped_data = Get_Context_Singleton()
                      ->get_device()
                      ->get_handle()
                      .mapMemory(memory,
                                 offset,
                                 VK_WHOLE_SIZE);
    return mapped_data;
}

void Buffer::Unmap()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .unmapMemory(memory);
}

std::vector<uint8_t> Buffer::Get_mapped_data(uint32_t offset)
{
    Map(offset, m_size);
    std::vector<uint8_t> dst_data(m_size);
    std::memcpy(dst_data.data(), mapped_data, m_size);
    Unmap();
    return dst_data;
}

void Buffer::Update(void* data, size_t size)
{

    Map(0, size);
    std::memcpy(mapped_data, data, size);
    if (m_permanent) {
        return;
    }
    Unmap();
}

void Buffer::Update(std::vector<void*> data, std::vector<size_t> size)
{

    assert(data.size() == size.size());
    size_t total_size = 0;
    for (auto i : size)
        total_size += i;
    Map(0, total_size);
    size_t cur_s = 0;
    for (auto i = 0; i < data.size(); i++) {
        auto d = data[i];
        auto s = size[i];

        std::memcpy((uint8_t*)mapped_data + cur_s, d, s);
        cur_s += s;
    }
    if (m_permanent) {
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
        Context::Get_Singleton()->get_debugger()->set_name(res, "mochengBuffer");
        return res;
    }
    std::shared_ptr<Buffer> host_buffer, device_buffer;
    host_buffer.reset(new Buffer(size,
                                 vk::BufferUsageFlagBits::eTransferSrc,
                                 vk::MemoryPropertyFlagBits::eHostVisible |
                                     vk::MemoryPropertyFlagBits::eHostCoherent));
    if (data) {
        std::vector<void*> d { data };
        std::vector<size_t> s { size };
        host_buffer->Update(d, s);
    }

    device_buffer.reset(new Buffer(size,
                                   vk::BufferUsageFlagBits::eTransferDst | usage,
                                   vk::MemoryPropertyFlagBits::eDeviceLocal,
                                   true));

    auto graphic_queue = Context::Get_Singleton()
                             ->get_device()
                             ->Get_Graphic_queue();

    CommandManager::ExecuteCmd(graphic_queue,
                               [&](auto& cmd_buffer) {
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

std::shared_ptr<Buffer> Buffer::CreateExternalBuffer(void* data, size_t size, vk::BufferUsageFlags usage_flags, vk::MemoryPropertyFlags properties, vk::ExternalMemoryHandleTypeFlagsKHR extMemHandleType)
{
#define VK_USE_CUDA
    // std::shared_ptr<Buffer> res(new Buffer(size, usage_flags, properties, extMemHandleType));
    // res->CreateBuffer(size, usage_flags);
#undef VK_USE_CUDA
}

vk::DeviceAddress Buffer::get_address()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    throw new std::runtime_error("android not support gpu address");
#else
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
#endif
}

void Buffer::CopyBuffer(std::shared_ptr<Buffer> src, std::shared_ptr<Buffer> dst)
{
    auto graphic_queue = Context::Get_Singleton()
                             ->get_device()
                             ->Get_Graphic_queue();

    CommandManager::ExecuteCmd(graphic_queue,
                               [&](auto& cmd_buffer) {
                                   vk::BufferCopy regin;
                                   regin.setSize(src->GetSize())
                                       .setDstOffset(0)
                                       .setSrcOffset(0);
                                   cmd_buffer.copyBuffer(
                                       src->get_handle(),
                                       dst->get_handle(),
                                       regin);
                               });
}

} // namespace MCRT
