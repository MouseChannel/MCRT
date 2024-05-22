#include "Helper/DescriptorSetTarget/BufferDescriptorSetTarget.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Sampler.hpp"
namespace MCRT {
BufferDescriptorSetTarget::BufferDescriptorSetTarget(std::shared_ptr<Buffer> buf,
                                                     //  Which_Set _which_set,
                                                     int binding_index,
                                                     vk::ShaderStageFlags shader_stage,
                                                     vk::DescriptorType type,
                                                     std::shared_ptr<DescriptorSet> descriptorSet,
                                                     int descriptorSet_index)
    : m_buffer(buf)
    , DescriptorSetTargetBase(
          // _which_set,
          binding_index,
          shader_stage,
          type,
          descriptorSet,
          descriptorSet_index)
{
}

void BufferDescriptorSetTarget::Update()
{

    std::vector<vk::DescriptorBufferInfo> buffer_infos;

    buffer_infos.emplace_back(vk::DescriptorBufferInfo()
                                  .setBuffer(m_buffer->get_handle())
                                  .setRange(m_buffer->GetSize())
                                  .setOffset(0));
    vk::WriteDescriptorSet writer;

    writer.setDescriptorType(m_type)
        .setBufferInfo(buffer_infos)
        .setDstBinding(m_binding_index)
        .setDstSet(m_descriptorSet->get_handle()[m_descriptorSet_index])
        .setDstArrayElement(0);
    Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .updateDescriptorSets(writer, {});
}

}