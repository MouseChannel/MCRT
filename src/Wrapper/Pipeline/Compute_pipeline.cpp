#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shader/Data_struct.h"

namespace MCRT {
Compute_Pipeline::Compute_Pipeline()
{

    auto descriptor_layout = Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Ray_Tracing);

    // auto layout_create_info = vk::PipelineLayoutCreateInfo().setPSetLayouts(&descriptor_layout);
    std::vector<vk::DescriptorSetLayout> descriptor_layouts {
        Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Ray_Tracing),
        Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Compute)

    };
    layout = Context::Get_Singleton()
                 ->get_device()
                 ->get_handle()
                 .createPipelineLayout(vk::PipelineLayoutCreateInfo()
                                           .setSetLayouts(descriptor_layouts)
                                           .setPushConstantRanges(vk::PushConstantRange()
                                                                      .setSize(sizeof(PushContant_Compute))
                                                                      .setStageFlags(vk::ShaderStageFlagBits::eCompute)));
    std::shared_ptr<ShaderModule> compute_shader {
        new ShaderModule("D:/MoChengRT/shader/test.comp.spv")
    };
    auto shader_stage = vk::PipelineShaderStageCreateInfo()
                            .setModule(compute_shader->get_handle())
                            .setStage(vk::ShaderStageFlagBits ::eCompute)
                            .setPName("main");
    auto create_info = vk::ComputePipelineCreateInfo()
                           .setLayout(layout)
                           .setStage(shader_stage);

    auto res = Context::Get_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createComputePipeline(nullptr, create_info);
    if (res.result != vk::Result::eSuccess) {
        throw std::runtime_error("fail to create compute pipeline");
    }
    m_handle = res.value;
}

}