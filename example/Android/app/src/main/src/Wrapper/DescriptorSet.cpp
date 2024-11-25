#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
namespace MoCheng3D {
DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorPool> descriptorPool, vk::DescriptorSetLayout setLayouts)
{
    vk::DescriptorSetAllocateInfo allocate_info;
    auto swapchain_size = Get_Context_Singleton()->Get_SwapChain()->Get_Swapchain_Image_size();
    std::vector<vk::DescriptorSetLayout> descriptor_layouts(swapchain_size, setLayouts);
    allocate_info.setDescriptorPool(descriptorPool->Get_handle())
        .setSetLayouts(descriptor_layouts);
//        .setDescriptorSetCount(1);
    m_handle = Get_Context_Singleton()->Get_Device()->Get_handle().allocateDescriptorSets(allocate_info);
}

DescriptorSet::~DescriptorSet()
{
}
void DescriptorSet::Update(std::shared_ptr<Buffer> buffer, uint32_t binding_index, vk::DescriptorType descriptor_type)
{
    for (int i = 0; i < m_handle.size(); i++) {

        auto set = m_handle[i];
        vk::DescriptorBufferInfo buffer_info;
        buffer_info
            .setBuffer(buffer->Get_handle())

            .setRange(buffer->GetSize())
            .setOffset(0);
        vk::WriteDescriptorSet writer;

        writer.setDescriptorType(descriptor_type)
            .setBufferInfo(buffer_info)
            .setDstBinding(binding_index)
            .setDstSet(set)
            .setDstArrayElement(0)
            .setDescriptorCount(1);
        Get_Context_Singleton()->Get_Device()->Get_handle().updateDescriptorSets(writer, {});
    }
}
void DescriptorSet::Update(std::shared_ptr<Image> image, uint32_t binding_index, vk::DescriptorType descriptor_type)
{
    for (int i = 0; i < m_handle.size(); i++) {

        auto set = m_handle[i];

        vk::WriteDescriptorSet writer;

        // std::unique_ptr<Sampler> sampler { new Sampler };
        auto sampler = Get_Context_Singleton()->Get_Sampler();
        vk::DescriptorImageInfo imageInfo;

        imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(image->Get_Image_View())
            .setSampler(sampler->Get_handle());
        writer.setImageInfo(imageInfo);

        writer.setDescriptorType(descriptor_type)

            .setDstBinding(binding_index)
            .setDstSet(set)
            .setDstArrayElement(0)
            .setDescriptorCount(1);
        Get_Context_Singleton()->Get_Device()->Get_handle().updateDescriptorSets(writer, {});
    }
}

} // namespace MoCheng3D