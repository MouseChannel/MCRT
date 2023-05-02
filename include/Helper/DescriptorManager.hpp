#pragma once
#include "Helper/Instance_base.hpp"
#include "Wrapper/Buffer.hpp"
#include <map>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Buffer;
class DescriptorPool;
class DescriptorSet;
class Image;
class AccelerationStructure;
class Descriptor_Manager : public Instance_base<Descriptor_Manager> {

    // template <typename T>
    struct Descriptor_Set_Binding {
    public:
       
        std::variant<std::shared_ptr<Buffer>, std::shared_ptr<Image>, std::shared_ptr<AccelerationStructure>> data;
        // std::shared_ptr<T> data;
        vk::DescriptorSetLayoutBinding binding;

        auto  get_data()
        {

            // if (auto data_buffer = std::get_if<buffer>(&data)) {
            //    return data_buffer->get();
            // }
            // if (auto data_image = std::get_if<image>(&data)) {
            //    return data_image->get();
            // }
            // if (auto data_buffer = std::get_if<buffer>(&data)) {
            //    return data_buffer->get();
            // }
        }
    };
    // struct DescriptorSet_Buffer_Binding : public Descriptor_Set_Binding<Buffer> {
    // };
    // struct DescriptorSet_Image_Binding : public Descriptor_Set_Binding<Image> {
    // };
    // struct DescriptorSet_RT_Binding : public Descriptor_Set_Binding<AccelerationStructure> {
    // };

public:
    vk::DescriptorSetLayout&
    Get_DescriptorSet_layout();
    Descriptor_Manager() = default;
    ~Descriptor_Manager();
    static std::unique_ptr<Descriptor_Manager> _instance;

    // static Descriptor_Manager& Get_Singleton();
    template <typename T>
    void Make_DescriptorSet(std::shared_ptr<T> data,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.setBinding(binding_index)
            .setDescriptorCount(1)
            .setStageFlags(shader_stage)
            .setDescriptorType(type);
        Descriptor_Set_Binding res;
        res.data = data;
        res.binding = binding;
        descriptor_set_binding.emplace_back(res);

        int a = 0;
    }

    // void Make_DescriptorSet(std::shared_ptr<Buffer> data,
    //                         uint32_t binding_index,
    //                         vk::DescriptorType type,
    //                         vk::ShaderStageFlags shader_stage);
    // void Make_DescriptorSet(std::shared_ptr<Image> data,
    //                         uint32_t binding_index,
    //                         vk::DescriptorType type,
    //                         vk::ShaderStageFlags shader_stage);
    // void Make_DescriptorSet(std::shared_ptr<AccelerationStructure> as_data,
    //                         uint32_t binding_index,
    //                         vk::DescriptorType type,
    //                         vk::ShaderStageFlags shader_stage);
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

    std::vector<Descriptor_Set_Binding> descriptor_set_binding; // std::map < vk::DescriptorType ,std::vector<>>
    // std::vector<DescriptorSet_Buffer_Binding> descriptorSet_buffer_binding_map;
    // std::vector<DescriptorSet_Image_Binding> descriptorSet_image_binding_map;
    // std::vector<DescriptorSet_RT_Binding> descriptorSet_rt_binding_map;
    // std::vector<class Tp>
    std::shared_ptr<DescriptorPool> descriptorPool;
    std::shared_ptr<DescriptorSet> descriptorSet;
    std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;
    vk::DescriptorSetLayout descriptor_layout { VK_NULL_HANDLE };
};

}