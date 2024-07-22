#include "Wrapper/Ray_Tracing/RaytracingPass.hpp"
#include "Rendering/RaytracingContext.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "shaders/PBR/IBL/push_constants.h"

namespace MCRT {
RaytracingPass::RaytracingPass()
{
}
void RaytracingPass::SetShaderModule(std::string shader_path, Stage stage)
{
    // m_shader_module.reset(new ShaderModule(shader_path));
}
void RaytracingPass::SetShaderModule(std::shared_ptr<ShaderModule> shader_module, Stage stage)
{
    switch (stage) {

    case E_GENERATE:
        m_gen_shader_modules.push_back(shader_module);
        break;
    case E_COLOESTHIT:
        m_hit_shader_modules.push_back(shader_module);
        break;
    case E_MISS:
        m_miss_shader_modules.push_back(shader_module);
        break;
    }
    // for (int i = 0; i < shaderEnum.raygen; i++) {
    //     m_gen_shader_modules
    // }
}
void RaytracingPass::create_shader_bind_table()
{
    auto device_properties = Context::Get_Singleton()
                                 ->get_device()
                                 ->Get_Physical_device()
                                 .getProperties2<vk::PhysicalDeviceProperties2,
                                                 vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    auto& rt_pipeline_properties = device_properties.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    // set up sbt data
    auto missCount { miss_shader_count };
    auto hitCount { hit_shader_count };
    // auto anyhitCount{ anyhit_shader_count };
    auto handleCount = 1 + missCount + hitCount;
    uint32_t handleSize = rt_pipeline_properties.shaderGroupHandleSize;

    uint32_t handleSizeAligned = align_up(handleSize, rt_pipeline_properties.shaderGroupHandleAlignment);

    m_rgenRegion.stride = align_up(handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    m_rgenRegion.size = m_rgenRegion.stride; // The size member of pRayGenShaderBindingTable must be equal to its stride member
    m_missRegion.stride = handleSizeAligned;
    m_missRegion.size = align_up(missCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    m_hitRegion.stride = handleSizeAligned;
    m_hitRegion.size = align_up(hitCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    // if (anyhitCount > 0) {
    //
    //     m_anyhitRegion.stride = handleSizeAligned;
    //     m_anyhitRegion.size = align_up(anyhitCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    // }

    // Get the shader group handles
    uint32_t dataSize = handleCount * handleSize;
    std::vector<uint8_t> handles(dataSize);
    // auto rt_pipeline = Context::Get_Singleton()->get_rt_pipeline();
    auto res = Context::Get_Singleton()
                   ->get_device()
                   ->get_handle()
                   .getRayTracingShaderGroupHandlesKHR(m_rt_pipeline->get_handle(),
                                                       0,
                                                       handleCount,
                                                       dataSize,
                                                       handles.data());
}
void RaytracingPass::prepare_pipeline(int pc_size)
{

    {
        std::vector<vk::PipelineShaderStageCreateInfo>
            stages;

        vk::RayTracingShaderGroupCreateInfoKHR shader_group_create_info;

        shader_group_create_info
            .setType(vk::RayTracingShaderGroupTypeKHR::eGeneral)
            .setAnyHitShader(VK_SHADER_UNUSED_KHR)
            .setClosestHitShader(VK_SHADER_UNUSED_KHR)
            .setIntersectionShader(VK_SHADER_UNUSED_KHR);
        for (int i = 0; i < m_shader_modules.size(); i++) {
            auto item_shader = m_shader_modules[i];
            switch (item_shader->get_stage()) {

            case vk::ShaderStageFlagBits::eRaygenKHR:
                shader_group_create_info.setGeneralShader(i);

            case vk::ShaderStageFlagBits::eClosestHitKHR:
                shader_group_create_info.setType(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup)
                    .setGeneralShader(VK_SHADER_UNUSED_KHR)
                    .setClosestHitShader(i);
            case vk::ShaderStageFlagBits::eMissKHR:
                shader_group_create_info.setGeneralShader(i);

                break;
            default:
                break;
            }

            m_groups.push_back(shader_group_create_info);

            stages.push_back(vk::PipelineShaderStageCreateInfo()
                                 .setStage(item_shader->get_stage())
                                 .setPName("main")
                                 .setModule(item_shader->get_handle()));
        }

        m_pipeline.reset(new RT_Pipeline(stages, m_groups));
        m_pipeline->Make_Layout(m_descriptorSet->get_layout(), pc_size, vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR | vk::ShaderStageFlagBits::eMissKHR);
        m_pipeline->Build();
        return;
    }

    std::vector<vk::PipelineShaderStageCreateInfo>
        stages;

    auto raytracing_context = Context::Get_Singleton()->get_rt_context();

    auto shaderEnum = raytracing_context->shaderEnum;
    auto rgen_index = 0;
    auto rhit_index = shaderEnum.raygen;
    auto rmiss_index = shaderEnum.raygen + shaderEnum.rclosestHit;
    for (int i = 0; i < shaderEnum.raygen; i++) {
        stages[i]
            .setPName("main")
            .setStage(vk::ShaderStageFlagBits::eRaygenKHR)
            .setModule(m_gen_shader_modules[i]->get_handle());
    }
    for (int i = 0; i < shaderEnum.rclosestHit; i++) {
        stages[i + rhit_index]
            .setPName("main")
            .setStage(vk::ShaderStageFlagBits::eClosestHitKHR)
            .setModule(m_hit_shader_modules[i]->get_handle());
    }

    for (int i = 0; i < shaderEnum.rmiss; i++) {
        stages[i + rmiss_index]
            .setPName("main")
            .setStage(vk::ShaderStageFlagBits::eMissKHR)
            .setModule(m_miss_shader_modules[i]->get_handle());
    }

    vk::RayTracingShaderGroupCreateInfoKHR create_info;

    create_info
        .setType(vk::RayTracingShaderGroupTypeKHR::eGeneral)
        .setAnyHitShader(VK_SHADER_UNUSED_KHR)
        .setClosestHitShader(VK_SHADER_UNUSED_KHR)
        .setIntersectionShader(VK_SHADER_UNUSED_KHR)
        .setGeneralShader(rgen_index);
    m_groups.push_back(create_info);
    for (int i = 0; i < shaderEnum.rclosestHit; i++) {
        create_info.setType(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup)
            .setGeneralShader(VK_SHADER_UNUSED_KHR)
            .setClosestHitShader(rhit_index + i);

        m_groups.push_back(create_info);
    }
    for (int i = 0; i < shaderEnum.rmiss; i++) {
        create_info.setGeneralShader(rmiss_index + i);
        m_groups.push_back(create_info);
    }
    m_pipeline.reset(new RT_Pipeline(stages, m_groups));
    m_pipeline->Make_Layout(m_descriptorSet->get_layout(), pc_size, vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR | vk::ShaderStageFlagBits::eMissKHR);
    m_pipeline->Build();
}

void RaytracingPass::raytrace(vk::CommandBuffer cmd)
{
    auto extent = Context::Get_Singleton()->get_extent2d();
    cmd.traceRaysKHR(m_rgenRegion,
                     m_missRegion,
                     m_hitRegion,
                     m_callRegion,
                     extent.width,
                     extent.height,
                     1);
}

// void RaytracingPass::Dispach(vk::CommandBuffer cmd, int local_size_x, int local_size_y, int local_size_z)
// {
//     cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
//                            m_pipeline->get_layout(),
//                            0,
//                            m_descriptorSet->get_handle(),
//                            {});
//     cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
//                      m_pipeline->get_handle());

//     cmd.dispatch(local_size_x, local_size_y, local_size_z);
// }

// void RaytracingPass::PreparePipeline()
// {
//     m_pipeline.reset(new Compute_Pipeline(m_shader_module));
//     m_pipeline->Make_Layout(m_descriptorSet->get_layout(), sizeof(PushContant_IBL), vk::ShaderStageFlagBits::eCompute);
//     m_pipeline->Build();
// }
// void RaytracingPass::PrepareDescriptorSet(std::function<void()> prepare)
// {
//      if (m_descriptorSet == nullptr)
//         m_descriptorSet.reset(new DescriptorSet());
//     prepare();
//     if (m_descriptorSet->check_dirty()) {

//         std::map<vk::DescriptorType, uint32_t> type_map;

//         for (auto& i : m_descriptorSetTarget) {
//             type_map[i->get_type()]++;
//         }

//         std::vector<vk::DescriptorPoolSize> pool_infos(type_map.size());
//         std::transform(type_map.begin(), type_map.end(), pool_infos.begin(), [](auto item) {
//             return vk::DescriptorPoolSize()
//                 .setDescriptorCount(item.second)
//                 .setType(item.first);
//         });

//         m_descriptorSetPool.reset(new DescriptorPool(pool_infos));
//         // allocate descriptorset

//         m_descriptorSet->create(m_descriptorSetPool, get_DescriptorSetCount());
//     }
//     for (auto& i : m_descriptorSetTarget) {
//         i->Update();
//     }

// }

}