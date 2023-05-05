#pragma once
#include "Helper/Instance_base.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
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

        auto get_data()
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

public:
    enum Which_Set {
        Graphic,
        Ray_Tracing,
        Set_Count
    };
    // vk::DescriptorSetLayout& Get_DescriptorSet_layout();
    //
    vk::DescriptorSetLayout& Get_DescriptorSet_layout(Which_Set which_set);
    // vk::DescriptorSetLayout get_RT_DescriptorSet_layout();
    Descriptor_Manager() = default;
    ~Descriptor_Manager();
    static std::unique_ptr<Descriptor_Manager> _instance;
    // static Descriptor_Manager& Get_Singleton();
    template <typename T>
    void Make_DescriptorSet(std::shared_ptr<T> data,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage,
                            Which_Set which_set)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.setBinding(binding_index)
            .setDescriptorCount(1)
            .setStageFlags(shader_stage)
            .setDescriptorType(type);
        // Descriptor_Set_Binding res;
        // res.data = data;
        // res.binding = binding;
        descriptor_set_binding[which_set].emplace_back(Descriptor_Set_Binding {
            .data { data },
            .binding { binding } });
        // descriptor_graphic_set_binding.emplace_back(res);

        // int a = 0;
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
    void CreateDescriptorPool(Which_Set which_set);
    void CreateUpdateDescriptorSet(Which_Set which_set);
    [[nodiscard("missing descriptor_set")]] auto get_DescriptorSet(Which_Set which_set)
    {
        return descriptorSets[which_set];
    }
    // [[nodiscard("missing descriptorPool")]] auto& get_descriptorpool()
    // {
    //     return descriptorPool;
    // }

private:
    // [[nodiscard]] std::vector<vk::DescriptorSetLayoutBinding>& Get_layout_bindings();
    //
    [[nodiscard]] std::vector<vk::DescriptorSetLayoutBinding>& Get_layout_bindings(Which_Set which_set);

    // [[nodiscard]] std::vector<vk::DescriptorSetLayoutBinding>& Get_RT_layout_bindings();

    // 0 = graphic; 1 = raytracing
    std::vector<std::vector<Descriptor_Set_Binding>> descriptor_set_binding { Set_Count };
    // std::vector<Descriptor_Set_Binding> descriptor_graphic_set_binding;
    // std::vector<Descriptor_Set_Binding> rt_descriptor_set_binding;

    // std::map < vk::DescriptorType ,std::vector<>>
    // std::vector<DescriptorSet_Buffer_Binding> descriptorSet_buffer_binding_map;
    // std::vector<DescriptorSet_Image_Binding> descriptorSet_image_binding_map;
    // std::vector<DescriptorSet_RT_Binding> descriptorSet_rt_binding_map;
    // std::vector<class Tp>
    std::vector<std::shared_ptr<DescriptorPool>> descriptorPools { Set_Count };

    // std::shared_ptr<DescriptorPool> descriptorPool;
    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets { Set_Count };
    // std::shared_ptr<DescriptorSet> descriptorSet;
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> layout_bindings { Set_Count };
    // std::vector<vk::DescriptorSetLayoutBinding> graphic_layout_bindings;

    // std::vector<vk::DescriptorSetLayoutBinding> rt_layout_bindings;
    std::vector<vk::DescriptorSetLayout> descriptor_set_layouts { Set_Count };
    // vk::DescriptorSetLayout graphic_descriptor_layout { VK_NULL_HANDLE };
    // vk::DescriptorSetLayout descriptor_RT_layout { VK_NULL_HANDLE };
};

}