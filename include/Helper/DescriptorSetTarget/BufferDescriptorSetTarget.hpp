
#pragma once
#include "Helper/DescriptorSetTarget/DescriptorSetTargetBase.hpp"
namespace MCRT {
class Buffer;
class BufferDescriptorSetTarget : public DescriptorSetTargetBase {
public:
    BufferDescriptorSetTarget(std::shared_ptr<Buffer> buf,
                              int binding_index,
                              vk::ShaderStageFlags shader_stage,
                              vk::DescriptorType type,
                              std::shared_ptr<DescriptorSet> descriptorSet,
                              int descriptorSet_index = 0);
    // BufferDescriptorSetTarget();
    void Update() override;
    int get_descriptorSet_count() override
    {
        return 1;
    }

private:
    std::shared_ptr<Buffer> m_buffer;
};

}