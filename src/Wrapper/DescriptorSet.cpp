#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include <map>

namespace MCRT {
DescriptorSet::DescriptorSet()
{
    int a = 0;
    // vk::DescriptorSetAllocateInfo allocate_info;
    // // auto swapchain_size = Get_Context_Singleton()->get_swapchain()->Get_Swapchain_Image_size();
    // std::vector<vk::DescriptorSetLayout> descriptor_layouts(10, setLayouts);
    // allocate_info.setDescriptorPool(descriptorPool->get_handle())
    //     .setSetLayouts(descriptor_layouts)
    //     // TODO swapchain_size
    //     .setDescriptorSetCount(1);
    // m_handle = Get_Context_Singleton()
    //                ->get_device()
    //                ->get_handle()
    //                .allocateDescriptorSets(allocate_info);
}

DescriptorSet::~DescriptorSet()
{
    
    Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .destroyDescriptorSetLayout(layout);
}
void DescriptorSet::add(std::vector<std::shared_ptr<Image>> data, vk::DescriptorSetLayoutBinding binding_data)
{

    _images[binding_data.binding] = Data_Binding(data, binding_data);
    if (!layout) {
        layout_bindings.push_back(binding_data);
    }
}
void DescriptorSet::add(std::vector<std::shared_ptr<Buffer>> data, vk::DescriptorSetLayoutBinding binding_data)
{

    _buffers[binding_data.binding] = Data_Binding(data, binding_data);
    // _buffers.emplace(std::pair {
    //     binding_data.binding,
    //     Data_Binding(data, binding_data) });
    if (!layout) {

        layout_bindings.push_back(binding_data);
    }
}
void DescriptorSet::add(std::vector<std::shared_ptr<AccelerationStructure_Top>> data, vk::DescriptorSetLayoutBinding binding_data)
{

    // as.push_back(Data_Binding(data, binding_data));

    _as[binding_data.binding] = Data_Binding(data, binding_data);
    // _as.emplace(std::pair {

    //     binding_data.binding,
    //     Data_Binding(data, binding_data) });
    if (!layout) {

        layout_bindings.push_back(binding_data);
    }
}
void DescriptorSet::update(std::vector<std::shared_ptr<Buffer>> new_data,
                           vk::DescriptorSetLayoutBinding binding_data,
                           int set_index)
{

    std::vector<vk::DescriptorBufferInfo> buffer_infos;

    for (auto& i : new_data) {
        buffer_infos.emplace_back(vk::DescriptorBufferInfo()
                                      .setBuffer(i->get_handle())
                                      .setRange(i->GetSize())
                                      .setOffset(0));
    }
    vk::WriteDescriptorSet writer;

    writer.setDescriptorType(binding_data.descriptorType)
        .setBufferInfo(buffer_infos)
        .setDstBinding(binding_data.binding)
        .setDstSet(get_handle()[set_index])
        .setDstArrayElement(0);
    Get_Context_Singleton()->get_device()->get_handle().updateDescriptorSets(writer, {});
}
void DescriptorSet::update(std::vector<std::shared_ptr<Image>> new_data,
                           vk::DescriptorSetLayoutBinding binding_data,
                           int set_index)
{
    std::vector<vk::DescriptorImageInfo> image_infos;
    for (auto& i : new_data) {
        image_infos.emplace_back(vk::DescriptorImageInfo()
                                     .setImageLayout(i->Get_image_layout())
                                     .setImageView(i->Get_Image_View())
                                     .setSampler(Context::Get_Singleton()->get_sampler()->get_handle()));
        // std::cout << i->height << i->width << std::endl;
        // std::cout << i->get_handle() << std::endl;
    }
    vk::WriteDescriptorSet writer;

    writer.setDescriptorType(binding_data.descriptorType)
        .setImageInfo(image_infos)
        .setDstBinding(binding_data.binding)
        .setDstSet(get_handle()[set_index])
        .setDstArrayElement(0);
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .updateDescriptorSets(writer, {});
}
void DescriptorSet::update(std::vector<std::shared_ptr<AccelerationStructure_Top>> new_data,
                           vk::DescriptorSetLayoutBinding binding_data,
                           int set_index)
{

    std::vector<vk::AccelerationStructureKHR> as_handles;
    for (auto& i : new_data) {
        as_handles.emplace_back(i->get_handle());
    }
    vk::WriteDescriptorSetAccelerationStructureKHR dst_as_info;
    dst_as_info
        .setAccelerationStructures(as_handles);

    vk::WriteDescriptorSet writer;

    writer.setDescriptorType(binding_data.descriptorType)

        .setDstBinding(binding_data.binding)
        .setDstSet(get_handle()[set_index])
        .setDstArrayElement(0)
        .setDescriptorCount(as_handles.size())
        .setPNext(&dst_as_info);
    Get_Context_Singleton()->get_device()->get_handle().updateDescriptorSets(writer, {});
}

void DescriptorSet::create(std::shared_ptr<DescriptorPool> pool, int size)
{

    vk::DescriptorSetAllocateInfo allocate_info;
    // auto swapchain_size = Get_Context_Singleton()->get_swapchain()->Get_Swapchain_Image_size();
    std::vector<vk::DescriptorSetLayout> descriptor_layouts(10, get_layout());
    allocate_info.setDescriptorPool(pool->get_handle())
        .setSetLayouts(descriptor_layouts)
        // TODO swapchain_size
        .setDescriptorSetCount(size);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .allocateDescriptorSets(allocate_info);
}

vk::DescriptorSetLayout DescriptorSet::get_layout()
{
    if (!layout) {
        vk::DescriptorSetLayoutCreateInfo layout_create_info;
        layout_create_info.setBindings(layout_bindings);
        layout = Context::Get_Singleton()
                     ->get_device()
                     ->get_handle()
                     .createDescriptorSetLayout(layout_create_info);
    }
    return layout;
}
} // namespace MoCheng3D