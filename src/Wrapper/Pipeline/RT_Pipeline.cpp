// #include "Wrapper/Pipeline/RT_pipeline.hpp"
// #include "Helper/DescriptorManager.hpp"
// #include "Rendering/RaytracingPass.hpp"
// #include "Wrapper/DescriptorSet.hpp"
// #include "Wrapper/Device.hpp"
// #include "Wrapper/Shader_module.hpp"
// #include "shaders/Data_struct.h"
// // #include "shaders/Set_binding.h"

// namespace MCRT {

//     RT_Pipeline::RT_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules,
//                              std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size)
//             : Pipeline_base(sets) {

//         auto rgen_shader_count{1};
//         auto rmiss_shader_count{Context::Get_Singleton()->get_rt_context()->get_miss_shader_count()};
//         auto rhit_shader_count{Context::Get_Singleton()->get_rt_context()->get_hit_shader_count()};
//         auto rahit_shader_count{Context::Get_Singleton()->get_rt_context()->get_anyhit_shader_count()};

//         auto rgen_index = 0;
//         auto rmiss_index = rgen_shader_count;
//         auto rhit_index = rgen_shader_count + rmiss_shader_count;
//         auto rahit_index = rhit_index + rhit_shader_count;
//         std::vector<vk::PipelineShaderStageCreateInfo>
//                 stages(rgen_shader_count + rmiss_shader_count + rhit_shader_count+rahit_shader_count);
//         stages[rgen_index]
//                 .setPName("main")
//                 .setStage(vk::ShaderStageFlagBits::eRaygenKHR)
//                 .setModule(shader_modules[eRaygen]->get_handle());

//         for (int i = 0; i < rmiss_shader_count; i++) {
//             stages[rmiss_index + i]
//                     .setPName("main")
//                     .setStage(vk::ShaderStageFlagBits::eMissKHR)
//                     .setModule(shader_modules[eMiss + i]->get_handle());
//         }

//         for (int i = 0; i < rhit_shader_count; i++) {
//             stages[rhit_index + i]
//                     .setPName("main")
//                     .setStage(vk::ShaderStageFlagBits::eClosestHitKHR)
//                     .setModule(shader_modules[eClosestHit + i]->get_handle());
//         }
//         for (int i = 0; i < rahit_shader_count; i++) {
//             stages[rahit_index + i]
//                     .setPName("main")
//                     .setStage(vk::ShaderStageFlagBits::eAnyHitKHR)
//                     .setModule(shader_modules[eAnyHit + i]->get_handle());
//         }

//         vk::RayTracingShaderGroupCreateInfoKHR create_info;
//         create_info
//                 .setType(vk::RayTracingShaderGroupTypeKHR::eGeneral)
//                 .setAnyHitShader(VK_SHADER_UNUSED_KHR)
//                 .setClosestHitShader(VK_SHADER_UNUSED_KHR)
//                 .setIntersectionShader(VK_SHADER_UNUSED_KHR)
//                 .setGeneralShader(rgen_index);
//         groups.push_back(create_info);
//         for (int i = 0; i < rmiss_shader_count; i++) {
//             create_info.setGeneralShader(rmiss_index + i);
//             groups.push_back(create_info);
//         }
//         for (int i = 0, j = 0; i < rhit_shader_count; i++, j++) {
//             create_info.setType(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup)
//                     .setGeneralShader(VK_SHADER_UNUSED_KHR)
//                     .setClosestHitShader(rhit_index + i);
//             if (j < rahit_shader_count) {
//                 create_info.setAnyHitShader(rahit_index + j);
//             }

//             groups.push_back(create_info);
//         }
// //    for (int i = 0; i < rahit_shader_count; i++) {
// //        create_info.setType(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup)
// //                   .setClosestHitShader(VK_SHADER_UNUSED_KHR)
// //                   .setGeneralShader(VK_SHADER_UNUSED_KHR)
// //                   .setAnyHitShader(rahit_index + i);
// //        groups.push_back(create_info);
// //    }
//         // pipeline layout
//         // vk::PushConstantRange push_contant;
//         // push_contant
//         //     .setStageFlags(vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR | vk::ShaderStageFlagBits::eMissKHR)
//         //     .setOffset(0)
//         //     .setSize(Context::Get_Singleton()->get_rt_context()->get_constants_size());

//         // std::vector<vk::DescriptorSetLayout> descriptor_layouts(Ray_Tracing_Set::ray_tracing_count);
//         // descriptor_layouts[Ray_Tracing_Set::e_ray_tracing] = Descriptor_Manager::Get_Singleton()
//         //                                                          ->Get_DescriptorSet_layout(Descriptor_Manager::Ray_Tracing);
//         // descriptor_layouts[Ray_Tracing_Set::e_ray_global] = Descriptor_Manager::Get_Singleton()
//         //                                                         ->Get_DescriptorSet_layout(Descriptor_Manager::Global);
//         // descriptor_sets.resize(Ray_Tracing_Set::ray_tracing_count);

//         // descriptor_sets[Ray_Tracing_Set::e_ray_tracing] = Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)->get_handle()[0];

//         // descriptor_sets[Ray_Tracing_Set::e_ray_global] = Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Global)->get_handle()[0];

//         vk::PipelineLayoutCreateInfo layout_create_info;

//         auto push_range = vk::PushConstantRange()
//                 .setStageFlags(
//                         vk::ShaderStageFlagBits::eRaygenKHR |
//                         vk::ShaderStageFlagBits::eClosestHitKHR |
//                         vk::ShaderStageFlagBits::eMissKHR)
//                 .setSize(push_constants_size);
//         layout_create_info.setSetLayouts(m_descriptor_layouts)
//                 .setPushConstantRanges(push_range);
//         layout = Context::Get_Singleton()
//                 ->get_device()
//                 ->get_handle()
//                 .createPipelineLayout(layout_create_info);
//         Context::Get_Singleton()->get_debugger()->set_handle_name(layout, "my_lyout");

//         vk::RayTracingPipelineCreateInfoKHR pipeline_create_info;
// #if defined(VK_USE_PLATFORM_ANDROID_KHR)
//         pipeline_create_info.setStages(stages)
//                 .setGroups(groups)
//                 .setLayout(layout)
//                 .setMaxPipelineRayRecursionDepth(
//                         2);
// #else
//         pipeline_create_info.setStages(stages)
//                 .setGroups(groups)
//                 .setLayout(layout)
//                 .setMaxPipelineRayRecursionDepth(
//                         Context::Get_Singleton()
//                                 ->get_device()
//                                 ->Get_Physical_device()
//                                 .getProperties2<vk::PhysicalDeviceProperties2KHR,
//                                         vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>()
//                                 .get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>()
//                                 .maxRayRecursionDepth);
//         std::cout << pipeline_create_info.maxPipelineRayRecursionDepth << std::endl;
// #endif

//         auto res = Context::Get_Singleton()
//                 ->get_device()
//                 ->get_handle()
//                 .createRayTracingPipelineKHR({}, {}, pipeline_create_info);
//         if (res.result != vk::Result::eSuccess) {
//             throw std::runtime_error("fail to create ray tracing pipeline");
//         }
//         m_handle = res.value;
//     }

//     RT_Pipeline::~RT_Pipeline() {
//     }
// }
