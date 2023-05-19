#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shader/Data_struct.h"

namespace MCRT {

RT_Pipeline::RT_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules)
{

    // shader_modules.resize(eShaderGroupCount);
    // shader_modules[eRaygen].reset(new ShaderModule("D:/MoChengRT/shader/Path_tracing/path_tracing.rgen.spv"));
    // shader_modules[eMiss].reset(new ShaderModule("D:/MoChengRT/shader/Path_tracing/path_tracing.rmiss.spv"));
    // shader_modules[eClosestHit].reset(new ShaderModule("D:/MoChengRT/shader/Path_tracing/path_tracing.rchit.spv"));

    std::vector<vk::PipelineShaderStageCreateInfo>
        stages(eShaderGroupCount);
    stages[eRaygen]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits ::eRaygenKHR)
        .setModule(shader_modules[eRaygen]->get_handle());
    stages[eMiss]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits ::eMissKHR)
        .setModule(shader_modules[eMiss]->get_handle());
    stages[eClosestHit]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits ::eClosestHitKHR)
        .setModule(shader_modules[eClosestHit]->get_handle());

    vk::RayTracingShaderGroupCreateInfoKHR create_info;
    create_info
        .setType(vk::RayTracingShaderGroupTypeKHR ::eGeneral)
        .setAnyHitShader(VK_SHADER_UNUSED_KHR)
        .setClosestHitShader(VK_SHADER_UNUSED_KHR)
        .setIntersectionShader(VK_SHADER_UNUSED_KHR)
        .setGeneralShader(eRaygen);
    groups.push_back(create_info);
    create_info.setGeneralShader(eMiss);
    groups.push_back(create_info);
    create_info.setType(vk::RayTracingShaderGroupTypeKHR ::eTrianglesHitGroup)
        .setClosestHitShader(eClosestHit)
        .setGeneralShader(VK_SHADER_UNUSED_KHR);
    groups.push_back(create_info);
    // pipeline layout
    vk::PushConstantRange push_contant;
    push_contant.setStageFlags(vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR | vk::ShaderStageFlagBits::eMissKHR)
        .setOffset(0)
        .setSize(sizeof(PushContant));
    vk::PipelineLayoutCreateInfo layout_create_info;

    std::vector<vk::DescriptorSetLayout> descriptor_layouts {
        Descriptor_Manager::Get_Singleton()
            ->Get_DescriptorSet_layout(Descriptor_Manager::Ray_Tracing),
        Descriptor_Manager::Get_Singleton()
            ->Get_DescriptorSet_layout(Descriptor_Manager::Global),

    };
    layout_create_info.setSetLayouts(descriptor_layouts)
        .setPushConstantRanges(push_contant);
    layout = Context::Get_Singleton()
                 ->get_device()
                 ->get_handle()
                 .createPipelineLayout(layout_create_info);
    // fix ray tracing data in pipeline
    vk::RayTracingPipelineCreateInfoKHR pipeline_create_info;
    pipeline_create_info.setStages(stages)
        .setGroups(groups)
        .setLayout(layout)
        .setMaxPipelineRayRecursionDepth(2);
    auto res = Context::Get_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createRayTracingPipelineKHR({}, {}, pipeline_create_info);
    if (res.result != vk::Result::eSuccess) {
        throw std::runtime_error("fail to create ray tracing pipeline");
    }
    m_handle = res.value;
}
RT_Pipeline::~RT_Pipeline()
{
}
}