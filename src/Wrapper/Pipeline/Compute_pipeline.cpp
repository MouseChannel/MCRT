#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shader/Data_struct.h"
#include "shader/Set_binding.h"

namespace MCRT {
Compute_Pipeline::Compute_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shaders)
{

    auto descriptor_layout = Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Ray_Tracing);

    // auto layout_create_info = vk::PipelineLayoutCreateInfo().setPSetLayouts(&descriptor_layout);
    std::vector<vk::DescriptorSetLayout> descriptor_layouts(Compute_Set::compute_count);
    descriptor_layouts[Compute_Set::e_compute] = Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Compute);
    descriptor_layouts[Compute_Set::e_comp_raytracing] = Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Ray_Tracing);
    descriptor_sets.resize(Compute_Set::compute_count);
    descriptor_sets[Compute_Set::e_compute] = Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute)->get_handle()[0];
    descriptor_sets[Compute_Set::e_comp_raytracing] = Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)->get_handle()[0];

    layout = Context::Get_Singleton()
                 ->get_device()
                 ->get_handle()
                 .createPipelineLayout(vk::PipelineLayoutCreateInfo()
                                           .setSetLayouts(descriptor_layouts)
                                           .setPushConstantRanges(vk::PushConstantRange()
                                                                      .setSize(sizeof(PushContant_Compute))
                                                                      .setStageFlags(vk::ShaderStageFlagBits::eCompute)));

    auto shader_stage = vk::PipelineShaderStageCreateInfo()
                            .setModule(shaders[0]->get_handle())
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