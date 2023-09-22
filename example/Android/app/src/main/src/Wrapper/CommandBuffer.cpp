#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/CommandPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Pipeline.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace MoCheng3D {
CommandBuffer::CommandBuffer()
{

    vk::CommandBufferAllocateInfo allocate_info;

    auto command_pool = Get_Context_Singleton()->Get_CommandPool();
    allocate_info.setCommandBufferCount(1)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(command_pool->Get_handle());

    m_handle = Get_Context_Singleton()->Get_Device()->Get_handle().allocateCommandBuffers(allocate_info)[0];
}
void CommandBuffer::Begin(vk::CommandBufferUsageFlags begin_flags)
{
    vk::CommandBufferBeginInfo begin_info;
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_handle.begin(begin_info);
}
void CommandBuffer::End()
{
    m_handle.end();
}
void CommandBuffer::Reset()
{
    m_handle.reset();
}
void CommandBuffer::BeginRenderPass(vk::RenderPassBeginInfo renderPassInfo, vk::SubpassContents content)
{
    m_handle.beginRenderPass(renderPassInfo, content);
}
void CommandBuffer::EndRenderPass()
{
    m_handle.endRenderPass();
}
void CommandBuffer::BindPipeline(vk::PipelineBindPoint type, std::shared_ptr<Pipeline> pipeline)
{
    m_handle.bindPipeline(type, pipeline->Get_handle());
}

void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    m_handle.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}
void CommandBuffer::DrawIndex(
    uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    m_handle.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
void CommandBuffer::CopyBuffer(std::shared_ptr<Buffer> src_buffer, std::shared_ptr<Buffer> dst_buffer)
{

    vk::BufferCopy regin;
    regin.setSize(src_buffer->GetSize()).setDstOffset(0).setSrcOffset(0);

    m_handle.copyBuffer(src_buffer->Get_handle(), dst_buffer->Get_handle(), regin);
}
void CommandBuffer::BindVertexBuffer(uint32_t firstBinding,
    std::vector<std::shared_ptr<Buffer>> buffers,
    vk::DeviceSize offset)
{
    std::vector<vk::Buffer> vkbuffers;
    std::vector<vk::DeviceSize> offsets;
    for (auto& i : buffers) {
        vkbuffers.push_back(i->Get_handle());
        offsets.push_back(offset);
    }
    m_handle.bindVertexBuffers(firstBinding, vkbuffers, offsets);
}
void CommandBuffer::BindIndicesBuffer(std::shared_ptr<Buffer> buffer, vk::DeviceSize offset, vk::IndexType index_type)
{
    m_handle.bindIndexBuffer(buffer->Get_handle(), offset, index_type);
}
void CommandBuffer::PushContants(
    vk::PipelineLayout layout, vk::ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, void* value)
{
    m_handle.pushConstants(layout, stageFlags, offset, size, value);
}
void CommandBuffer::BindDescriptorSet(vk::PipelineLayout layout, vk::DescriptorSet descriptorset)
{
    m_handle.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, descriptorset, {});
}
void CommandBuffer::Push_Constants(
    vk::PipelineLayout layout, vk::ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* data)
{
    m_handle.pushConstants(layout, stageFlags, offset, size, data);
}

CommandBuffer::~CommandBuffer()
{
    auto command_pool = Get_Context_Singleton()->Get_CommandPool();
    Get_Context_Singleton()->Get_Device()->Get_handle().freeCommandBuffers(command_pool->Get_handle(), m_handle);
}

} // namespace MoCheng3D