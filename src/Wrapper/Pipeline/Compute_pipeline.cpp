#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shaders/Data_struct.h"
// #include "shaders/Set_binding.h"

namespace MCRT {

// Compute_Pipeline::Compute_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shaders, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size)
//     : Pipeline_base(sets)
// {

//     // layout = Context::Get_Singleton()
//     //              ->get_device()
//     //              ->get_handle()
//     //              .createPipelineLayout(vk::PipelineLayoutCreateInfo()
//     //                                        .setSetLayouts(m_descriptor_layouts)
//     //                                        .setPushConstantRanges(vk::PushConstantRange()
//     //                                                                   .setSize(push_constants_size)
//     //                                                                   .setStageFlags(vk::ShaderStageFlagBits::eCompute)));

//     auto shader_stage = vk::PipelineShaderStageCreateInfo()
//                             .setModule(shaders[0]->get_handle())
//                             .setStage(vk::ShaderStageFlagBits ::eCompute)
//                             .setPSpecializationInfo(&shaders[0]->get_specialinfo())
//                             .setPName("main");
//     auto create_info = vk::ComputePipelineCreateInfo()
//                            .setLayout(pipelineLayout)
//                            .setStage(shader_stage);

//     auto res = Context::Get_Singleton()
//                    ->get_device()
//                    ->get_handle()
//                    .createComputePipeline(nullptr, create_info);
//     if (res.result != vk::Result::eSuccess) {
//         throw std::runtime_error("fail to create compute pipeline");
//     }
//     m_handle = res.value;
// }
Compute_Pipeline::Compute_Pipeline(std::shared_ptr<ShaderModule> shader)
{
    shader_stage = vk::PipelineShaderStageCreateInfo()
                       .setModule(shader->get_handle())
                       .setStage(vk::ShaderStageFlagBits ::eCompute)
                       .setPSpecializationInfo(&shader->get_specialinfo())
                       .setPName("main");
}
// Compute_Pipeline::Compute_Pipeline(std::shared_ptr<ShaderModule> shaders, std::shared_ptr<DescriptorSet> sets, int push_constants_size)
//     : Pipeline_base(sets)
// {
// }
void Compute_Pipeline::Build()
{
    auto create_info = vk::ComputePipelineCreateInfo()
                           .setLayout(pipelineLayout)
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

Compute_Pipeline::~Compute_Pipeline()
{
    Get_Context_Singleton()->get_device()->get_handle().destroyPipelineLayout(pipelineLayout);
    Get_Context_Singleton()->get_device()->get_handle().destroyPipeline(m_handle);
}
}