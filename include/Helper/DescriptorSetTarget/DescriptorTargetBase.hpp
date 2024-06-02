
#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
namespace MCRT {
class DescriptorSet;
enum Which_Set {
    Graphic,
    Ray_Tracing,
    Compute,
    Global,

    e_type_count
};
class DescriptorTargetBase {
public:
    DescriptorTargetBase(
        int binding_index,
        vk::ShaderStageFlags shader_stage,
        vk::DescriptorType type,
        std::shared_ptr<DescriptorSet> descriptorSet,
        int descriptorSet_index)
        : m_binding_index(binding_index + descriptorSet_index * MAX_DESCRIPTORSET_COUNT)
        , m_shader_stage(shader_stage)
        , m_type(type)
        , m_descriptorSet(descriptorSet)
        , m_descriptorSet_index(descriptorSet_index)
    {
    }

    virtual void Update() = 0;
    auto get_type()
    {
        return m_type;
    }
    auto get_binding_index()
    {
        return m_binding_index;
    }
    auto get_shader_stage()
    {
        return m_shader_stage;
    }
    virtual int get_descriptor_count() = 0;
    const static int MAX_DESCRIPTORSET_COUNT = 1000;
     
protected:
    int m_binding_index;
    vk::ShaderStageFlags m_shader_stage;
    vk::DescriptorType m_type;
    int m_descriptorSet_index = 0;

    std::shared_ptr<DescriptorSet> m_descriptorSet;
};
}