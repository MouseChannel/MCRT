
#pragma once
#include "Helper/DescriptorSetTarget/DescriptorTargetBase.hpp"
namespace MCRT {
class Buffer;
class BufferDescriptorTarget : public DescriptorTargetBase {
public:
    BufferDescriptorTarget(std::shared_ptr<Buffer> buf,
                              int binding_index,
                              vk::ShaderStageFlags shader_stage,
                              vk::DescriptorType type,
                              std::shared_ptr<DescriptorSet> descriptorSet,
                              int descriptorSet_index = 0);
    // BufferDescriptorSetTarget();
    void Update() override;
    int get_descriptor_count() override
    {
        return 1;
    }

private:
    std::shared_ptr<Buffer> m_buffer;
};

}