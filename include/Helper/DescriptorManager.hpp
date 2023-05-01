#pragma once
#include "Helper/Instance_base.hpp"
#include <map>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Buffer;
class DescriptorPool;
class DescriptorSet;
class Image;
class Descriptor_Manager : public Instance_base<Descriptor_Manager> {

    struct DescriptorSet_Buffer_Binding {
        std::shared_ptr<Buffer> buffer;

        vk::DescriptorSetLayoutBinding binding;
    };
    struct DescriptorSet_Image_Binding {

        std::shared_ptr<Image> image;
        vk::DescriptorSetLayoutBinding binding;
    };
    struct DescriptorSet_RT_Binding {
        vk::AccelerationStructureKHR as_data;
        vk::DescriptorSetLayoutBinding binding;
    };

public:
    vk::DescriptorSetLayout&
    Get_DescriptorSet_layout();
    Descriptor_Manager() = default;
    ~Descriptor_Manager();
    static std::unique_ptr<Descriptor_Manager> _instance;

    // static Descriptor_Manager& Get_Singleton();

    void Make_DescriptorSet(std::shared_ptr<Buffer> data,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage);
    void Make_DescriptorSet(std::shared_ptr<Image> data,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage);
    void Make_DescriptorSet(vk::AccelerationStructureKHR as_data,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage);
    void CreateDescriptorPool();
    void CreateUpdateDescriptorSet();
    [[nodiscard("missing descriptor_set")]] auto& Get_DescriptorSet()
    {
        return descriptorSet;
    }
    [[nodiscard("missing descriptorPool")]] auto& get_descriptorpool()
    {
        return descriptorPool;
    }

private:
    [[nodiscard]] std::vector<vk::DescriptorSetLayoutBinding>& Get_layout_bindings();

    std::vector<DescriptorSet_Buffer_Binding> descriptorSet_buffer_binding_map;
    std::vector<DescriptorSet_Image_Binding> descriptorSet_image_binding_map;
    std::vector<DescriptorSet_RT_Binding> descriptorSet_rt_binding_map;
    // std::vector<class Tp>
    std::shared_ptr<DescriptorPool> descriptorPool;
    std::shared_ptr<DescriptorSet> descriptorSet;
    std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;
    vk::DescriptorSetLayout descriptor_layout { VK_NULL_HANDLE };
};

}