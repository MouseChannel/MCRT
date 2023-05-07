#pragma once
#include "Helper/Instance_base.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Component.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include <any>
#include <map>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Buffer;
class DescriptorPool;
class DescriptorSet;
class Image;
class AccelerationStructure;
class Descriptor_Manager : public Instance_base<Descriptor_Manager> {

public:
    enum Which_Set {
        Graphic,
        Ray_Tracing,
        Global,
        e_type_count
    };
    enum Type {
        e_undefined,
        e_buffer,
        e_image,
        e_tlas,
        // e_type_count
    };
    // template <typename T>
    struct Descriptor_Set_Binding {
    public:
        Descriptor_Set_Binding(std::any data,
                               vk::DescriptorSetLayoutBinding binding,
                               Type type)
            : data(data)
            , binding(binding)
            , type(type)
        {
        }
        std::any data;
        vk::DescriptorSetLayoutBinding binding;
        Type type { e_undefined };
    };

    // vk::DescriptorSetLayout& Get_DescriptorSet_layout();
    //
    vk::DescriptorSetLayout& Get_DescriptorSet_layout(Which_Set which_set);
    // vk::DescriptorSetLayout get_RT_DescriptorSet_layout();
    Descriptor_Manager() = default;
    ~Descriptor_Manager();
    // static std::unique_ptr<Descriptor_Manager> _instance;
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
        // descriptor_set_binding[]
        // add_descriptor_set_binding(which_set, data, binding);
        descriptor_set_binding[which_set].push_back({ data, binding, e_buffer });
    }
    template <typename T>
    void add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<T> data, vk::DescriptorSetLayoutBinding binding)
    {
        add_descriptor_set_binding(which_set, data, binding);
    }
    void add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<Buffer> data, vk::DescriptorSetLayoutBinding binding);
    void add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<Image> data, vk::DescriptorSetLayoutBinding binding);
    void add_descriptor_set_binding(Which_Set which_set, std::shared_ptr<AccelerationStructure> data, vk::DescriptorSetLayoutBinding binding);

    void CreateDescriptorPool(Which_Set which_set);
    void CreateUpdateDescriptorSet(Which_Set which_set);
    [[nodiscard("missing descriptor_set")]] auto get_DescriptorSet(Which_Set which_set)
    {
        return descriptorSets[which_set];
    }

private:
    [[nodiscard]] std::vector<vk::DescriptorSetLayoutBinding>& Get_layout_bindings(Which_Set which_set);

    // 0 = graphic; 1 = raytracing

    // std::vector<Descriptor_Set_Binding<Buffer>> descriptor_set_buffer_binding;
    // std::vector<Descriptor_Set_Binding<Image>> descriptor_set_image_binding;
    // std::vector<Descriptor_Set_Binding<AccelerationStructure>> descriptor_set_tlas_binding;

    std::vector<
        std::vector<
            Descriptor_Set_Binding>>
        descriptor_set_binding { e_type_count };
    std::vector<std::shared_ptr<DescriptorPool>> descriptorPools { e_type_count };

    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets { e_type_count };
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> layout_bindings { e_type_count };

    std::vector<vk::DescriptorSetLayout> descriptor_set_layouts { e_type_count };
};

}