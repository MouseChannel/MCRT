#pragma once
#include "Helper/Instance_base.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Wrapper//Ray_Tracing/AS_top.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Component.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include <any>
#include <map>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Buffer;
class DescriptorPool;
class Image;
class Descriptor_Manager : public Instance_base<Descriptor_Manager> {

public:
    enum Which_Set {
        Graphic,
        Ray_Tracing,
        Global,
        Compute,
        e_type_count
    };
    enum Type {
        e_undefined,
        e_buffer,
        e_image,
        e_tlas,
        // e_type_count
    };
    vk::DescriptorSetLayout Get_DescriptorSet_layout(Which_Set which_set);
    Descriptor_Manager() = default;
    ~Descriptor_Manager();
    template <typename T>
    void Make_DescriptorSet(std::vector<std::shared_ptr<T>> data,
                            Which_Set which_set,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.setBinding(binding_index)
            .setDescriptorCount(data.size())
            .setStageFlags(shader_stage)
            .setDescriptorType(type);

        if (!descriptorsets[which_set])
            descriptorsets[which_set].reset(new DescriptorSet);
        descriptorsets[which_set]->Add(data, binding);
    }
    template <typename T>
    void Make_DescriptorSet(std::shared_ptr<T> data,
                            Which_Set which_set,
                            uint32_t binding_index,
                            vk::DescriptorType type,
                            vk::ShaderStageFlags shader_stage)
    {
        Make_DescriptorSet(std::vector { data }, which_set, binding_index, type, shader_stage);
    }

    template <typename T>
    void Make_DescriptorSet(std::shared_ptr<Uniform_Stuff<T>> uniform_data,
                            uint32_t binding_index,
                            Which_Set which_set)
    {
        Make_DescriptorSet(std::vector { uniform_data->buffer }, which_set, binding_index, uniform_data->type, uniform_data->shader_stage);
    }
    void update_descriptor_set(Which_Set which_set);
    void CreateDescriptorPool(Which_Set which_set);
    // void CreateUpdateDescriptorSet(Which_Set which_set);
    [[nodiscard("missing descriptor_set")]] auto& get_DescriptorSet(Which_Set which_set)
    {
        return descriptorsets[which_set];
    }

private:
    std::vector<std::shared_ptr<DescriptorPool>> descriptorPools { e_type_count };

    std::vector<std::unique_ptr<DescriptorSet>> descriptorsets { e_type_count };
};

}