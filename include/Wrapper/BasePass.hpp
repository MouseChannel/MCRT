#pragma once

// #include "Helper/DescriptorManager.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "vulkan/vulkan.hpp"
#include <functional>
#include <memory>
namespace MCRT {
class DescriptorSetTargetBase;
class DescriptorPool;
class BasePass {
public:
    BasePass() = default;

    virtual ~BasePass() = default;

    virtual void prepare_pipeline(int pc_size) = 0;

   
     

    virtual void Prepare_DescriptorSet(std::function<void()>);
    void AddDescriptorSetTarget(std::shared_ptr<DescriptorSetTargetBase> target);
    // void AddDescriptorSetTarget(std::vector<std::shared_ptr<DescriptorSetTargetBase> >targets);

    vk::SubpassDescription description;
    virtual int get_DescriptorSetCount();
   
    auto& get_DescriptorSet()
    {
        return m_descriptorSet;
    }

protected:
 
    // std::shared_ptr<Graphic_Pipeline> m_pipeline;
    std::shared_ptr<DescriptorSet> m_descriptorSet;
    std::shared_ptr<DescriptorPool> m_descriptorSetPool;
    std::map<int, std::shared_ptr<DescriptorSetTargetBase>>m_descriptorSetTargetMap;
    // std::vector<std::shared_ptr<DescriptorSetTargetBase>> m_descriptorSetTarget;
};
}
