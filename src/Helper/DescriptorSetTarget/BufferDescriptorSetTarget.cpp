#include "Helper/DescriptorSetTarget/BufferDescriptorTarget.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Sampler.hpp"
namespace MCRT {
BufferDescriptorTarget::BufferDescriptorTarget(std::shared_ptr<Buffer> buf,
                                               //  Which_Set _which_set,
                                               int binding_index,
                                               vk::ShaderStageFlags shader_stage,
                                               vk::DescriptorType type,
                                                DescriptorSet* descriptorSet )
    : m_buffer(buf)
    , DescriptorTargetBase(
          // _which_set,
          binding_index,
          shader_stage,
          type,
          descriptorSet )
{
}

void BufferDescriptorTarget::Update()
{

    std::vector<vk::DescriptorBufferInfo> buffer_infos;

    buffer_infos.emplace_back(vk::DescriptorBufferInfo()
                                  .setBuffer(m_buffer->get_handle())
                                  .setRange(m_buffer->GetSize())
                                  .setOffset(0));
    vk::WriteDescriptorSet writer;
    for (auto i : m_descriptorSet->get_handle()) {

        writer.setDescriptorType(m_type)
            .setBufferInfo(buffer_infos)
            .setDstBinding(m_binding_index)
            .setDstSet(i)
            .setDstArrayElement(0);
        Context::Get_Singleton()
            ->get_device()
            ->get_handle()
            .updateDescriptorSets(writer, {});
    }
}

}