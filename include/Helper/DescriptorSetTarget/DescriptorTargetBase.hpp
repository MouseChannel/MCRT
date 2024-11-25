
#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>
namespace MCRT {
class DescriptorSet;
 
class DescriptorTargetBase {
public:
    DescriptorTargetBase(
        int binding_index,
        vk::ShaderStageFlags shader_stage,
        vk::DescriptorType type,
         DescriptorSet* descriptorSet )
        : m_binding_index(binding_index  )
        , m_shader_stage(shader_stage)
        , m_type(type)
        , m_descriptorSet(descriptorSet)
        // , m_descriptorSet_index(descriptorSet_index)
    {
    }

    virtual void Update() = 0;
    auto  get_type()
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
    // const static int MAX_DESCRIPTORSET_COUNT = 1000;
     
    vk::DescriptorType m_type;
protected:
    int m_binding_index;
    vk::ShaderStageFlags m_shader_stage;
    // int m_descriptorSet_index = 0;

    DescriptorSet* m_descriptorSet;

    // std::vector<std::shared_ptr<DescriptorSet>> m_descriptorSets;
};
}