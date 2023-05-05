#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"

namespace MCRT {
DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorPool> descriptorPool, vk::DescriptorSetLayout setLayouts)
{
    vk::DescriptorSetAllocateInfo allocate_info;
    auto swapchain_size = Get_Context_Singleton()->get_swapchain()->Get_Swapchain_Image_size();
    std::vector<vk::DescriptorSetLayout> descriptor_layouts(swapchain_size, setLayouts);
    allocate_info.setDescriptorPool(descriptorPool->get_handle())
        .setSetLayouts(descriptor_layouts)
        // TODO swapchain_size
        .setDescriptorSetCount(1);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .allocateDescriptorSets(allocate_info);
}

DescriptorSet::~DescriptorSet()
{
}

void DescriptorSet::Update(std::variant<std::shared_ptr<Buffer>,
                                        std::shared_ptr<Image>,
                                        std::shared_ptr<AccelerationStructure>> data,
                           uint32_t binding_index,
                           vk::DescriptorType descriptor_type)
{
    enum { buffer,
           image,
           as };
    // auto& ee = data;
    // auto data_buffer = std::get<0>(data);
    if (auto data_buffer = std::get_if<buffer>(&data)) {
        Update((std::shared_ptr<Buffer>)data_buffer->get(),
               binding_index,
               descriptor_type);
    } else if (auto data_image = std::get_if<image>(&data)) {
        Update((std::shared_ptr<Image>)data_image->get(),
               binding_index,
               descriptor_type);

    } else if (auto data_as = std::get_if<as>(&data)) {
        Update((std::shared_ptr<AccelerationStructure>)data_as->get(),
               binding_index,
               descriptor_type);
    }
}
void DescriptorSet::Update(std::shared_ptr<Buffer> buffer, uint32_t binding_index, vk::DescriptorType descriptor_type)
{
    for (int i = 0; i < m_handle.size(); i++) {

        auto set = m_handle[i];
        vk::DescriptorBufferInfo buffer_info;
        buffer_info
            .setBuffer(buffer->get_handle())

            .setRange(buffer->GetSize())
            .setOffset(0);
        vk::WriteDescriptorSet writer;

        writer.setDescriptorType(descriptor_type)
            .setBufferInfo(buffer_info)
            .setDstBinding(binding_index)
            .setDstSet(set)
            .setDstArrayElement(0)
            .setDescriptorCount(1);
        Get_Context_Singleton()->get_device()->get_handle().updateDescriptorSets(writer, {});
    }
}
void DescriptorSet::Update(std::shared_ptr<Image> image, uint32_t binding_index, vk::DescriptorType descriptor_type)
{
    for (int i = 0; i < m_handle.size(); i++) {

        auto set = m_handle[i];

        vk::WriteDescriptorSet writer;

        // std::unique_ptr<Sampler> sampler { new Sampler };
        auto sampler = Get_Context_Singleton()->get_sampler();
        vk::DescriptorImageInfo imageInfo;

        imageInfo.setImageLayout(image->Get_image_layout())
            .setImageView(image->Get_Image_View())
            .setSampler(sampler->get_handle());
        writer.setImageInfo(imageInfo);

        writer.setDescriptorType(descriptor_type)
            .setDstBinding(binding_index)
            .setDstSet(set)
            .setDstArrayElement(0)
            .setDescriptorCount(1);
        Get_Context_Singleton()
            ->get_device()
            ->get_handle()
            .updateDescriptorSets(writer, {});
    }
}

void DescriptorSet::Update(std::shared_ptr<AccelerationStructure> acceleration_structure, uint32_t binding_index, vk::DescriptorType descriptor_type)
{
    vk::WriteDescriptorSetAccelerationStructureKHR dst_as_info;
    dst_as_info.setAccelerationStructureCount(1)
        .setAccelerationStructures(acceleration_structure->get_handle());
    for (auto i = 0; i < m_handle.size(); i++) {
        auto set = m_handle[i];
        vk::WriteDescriptorSet writer;
        writer.setDescriptorType(descriptor_type)
            .setDstBinding(binding_index)
            .setDstSet(set)
            .setDescriptorCount(1)
            .setDstArrayElement(0)
            .setPNext(&dst_as_info);
        Get_Context_Singleton()
            ->get_device()
            ->get_handle()
            .updateDescriptorSets(writer, {});
    }
}
} // namespace MoCheng3D