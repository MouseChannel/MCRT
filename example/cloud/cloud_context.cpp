#include "example/cloud/cloud_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/ComputePass.hpp"
#include "Rendering/GraphicPass.hpp"
#include "Rendering/PBR/IBL_Manager.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/Skybox.hpp"
#include "Wrapper/Texture.hpp"
#include "example/base/shaders/ray_tracing/Data_struct.h"
#include "example/cloud/shader/Binding.h"
#include "example/cloud/shader/Push_Constants.h"
#include "iostream"
#include "shaders/PBR/IBL/binding.h"
#include "Rendering/Noise/NoiseManager.hpp"
// #include ""

namespace MCRT {
    std::unique_ptr<Context> Context::_instance{new MCRT::cloud_context};
    float cloud_context::noise_scale = 1.f;
    float cloud_context::thickness = 1.5f;
    float cloud_context::speed = 1.f;
    float cloud_context::light_pos_x = 0, cloud_context::light_pos_y = 0, cloud_context::light_pos_z = -5;


    int irradiance_size = 512;

    cloud_context::cloud_context() {
    }

    cloud_context::~cloud_context() {
    }

    void cloud_context::prepare(std::shared_ptr<Window> window) {
        ray_tracing_context::prepare(window);
        m_skybox.reset(new Skybox("/home/mocheng/project/MCRT/assets/Cubemap/moonless_golf_8k.hdr"));
        skybox_mesh = GLTF_Loader::load_skybox("/home/mocheng/project/MCRT/assets/cube.gltf");
        // GLTF_Loader::load_model("/home/mocheng/project/MCRT/assets/pbr/cardbox.gltf");

        // GLTF_Loader::load_model("/home/mocheng/project/MCRT/assets/pbr/elephant.gltf");
        GLTF_Loader::load_model("/home/mocheng/project/MCRT/assets/delete.glb");
        // GLTF_Loader::load_model("/home/mocheng/Desktop/untitled.gltf");

        // IBLManager::Get_Singleton()->Init(m_skybox);
        NoiseManager::Get_Singleton()->InitPerlinNoise();
        NoiseManager::Get_Singleton()->pre_compute_PerlinNoise();
//        NoiseManager::Get_Singleton()->get_noise_image()->SetImageLayout(
//                vk::ImageLayout::e
//                )

        PASS.resize(3);

        {
            // raytracing

            PASS[Ray_tracing] = std::shared_ptr<RaytracingPass>{new RaytracingPass(m_device)};
            std::vector<std::shared_ptr<ShaderModule>> rt_shader_modules(RT_Pipeline::eShaderGroupCount);
            rt_shader_modules[RT_Pipeline::eRaygen].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/cloud.rgen.spv"));
            // regular miss
            rt_shader_modules[RT_Pipeline::eMiss].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/cloud.rmiss.spv"));
            // ray marching cloud density
            rt_shader_modules[RT_Pipeline::eMiss2].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/ray_marching_cloud.rmiss.spv"));
            // ray marching light density
            rt_shader_modules[RT_Pipeline::eMiss3].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/ray_marching_light.rmiss.spv"));
            rt_shader_modules[RT_Pipeline::eClosestHit].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/cloud_front.rchit.spv"));
            rt_shader_modules[RT_Pipeline::eClosestHit2].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/cloud_behind.rchit.spv"));
//            rt_shader_modules[RT_Pipeline::eClosestHit3].reset(
//                    new ShaderModule("/home/mocheng/project/MCRT/example/cloud/shader/light_behind.rchit.spv"));
            Context::Get_Singleton()->get_rt_context()->set_hit_shader_count(2);
//          Context::Get_Singleton()->get_rt_context()->set_anyhit_shader_count(1);

            Context::Get_Singleton()->get_rt_context()->set_miss_shader_count(2);

            Context::Get_Singleton()->get_rt_context()->set_constants_size(sizeof(PushContant_Cloud));
            PASS[Ray_tracing]->prepare();
            PASS[Ray_tracing]->prepare_descriptorset([&]() {
                auto rt_context = Context::Get_Singleton()->get_rt_context();
                auto descriptor_manager = rt_context->get_descriptor_manager();


                descriptor_manager
                        ->Make_DescriptorSet(std::vector{AS_Builder::Get_Singleton()->get_tlas()},
                                             DescriptorManager::Ray_Tracing,
                                             (int) Ray_Tracing_Binding::e_tlas,
                                             vk::DescriptorType::eAccelerationStructureKHR,
                                             vk::ShaderStageFlagBits::eRaygenKHR |
                                             vk::ShaderStageFlagBits::eClosestHitKHR |
                                             vk::ShaderStageFlagBits::eMissKHR);

                descriptor_manager
                        ->Make_DescriptorSet(std::vector{rt_context->get_out_image()},
                                             DescriptorManager::Ray_Tracing,
                                             (int) Ray_Tracing_Binding::e_out_image,
                                             vk::DescriptorType::eStorageImage,
                                             vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
                descriptor_manager
                        ->Make_DescriptorSet(std::vector{NoiseManager::Get_Singleton()->get_noise_image()},
                                             DescriptorManager::Ray_Tracing,
                                             (int) Ray_Tracing_Binding::e_noise_data,
                                             vk::DescriptorType::eCombinedImageSampler,
                                             vk::ShaderStageFlagBits::eMissKHR |
                                             vk::ShaderStageFlagBits::eClosestHitKHR);
                descriptor_manager
                        ->Make_DescriptorSet(std::vector{m_skybox->get_handle()},
                                             DescriptorManager::Ray_Tracing,
                                             (int) Ray_Tracing_Binding::e_rt_skybox,
                                             vk::DescriptorType::eCombinedImageSampler,
                                             vk::ShaderStageFlagBits::eMissKHR |
                                             vk::ShaderStageFlagBits::eClosestHitKHR);

//                descriptor_manager
//                        ->Make_DescriptorSet(
//                                NoiseManager::Get_Singleton()->get_permutations(),
//                                (int) Ray_Tracing_Binding:: e_test,
//                                DescriptorManager::Ray_Tracing);
//                descriptor_manager
//                        ->Make_DescriptorSet(
//                                std::vector{IBLManager::Get_Singleton()->get_irradiance()->get_handle()},
//                                DescriptorManager::Ray_Tracing,
//                                (int) Ray_Tracing_Binding::e_rt_irradiance_image,
//                                vk::DescriptorType::eCombinedImageSampler,
//                                vk::ShaderStageFlagBits::eClosestHitKHR);
            });
            auto sets = std::vector<std::shared_ptr<DescriptorSet >>((int) Ray_Tracing_Set::ray_tracing_count);
            sets[(int) Ray_Tracing_Set::e_ray_tracing] = PASS[Ray_tracing]->get_descriptor_manager()->get_DescriptorSet(
                    DescriptorManager::Ray_Tracing);
            sets[(int) Ray_Tracing_Set::e_ray_global] = PASS[Ray_tracing]->get_descriptor_manager()->get_DescriptorSet(
                    DescriptorManager::Global);
            PASS[Ray_tracing]->prepare_pipeline(rt_shader_modules,
                                                sets,
                                                sizeof(PushContant_Cloud));
            PASS[Ray_tracing]->post_prepare();
        }

        {
            // graphic
            PASS[Graphic] = std::shared_ptr<GraphicPass>{new GraphicPass(m_device)};
            PASS[Graphic]->set_constants_size(sizeof(PushContant));

            std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
            graphic_shader_modules[Graphic_Pipeline::Main_VERT].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/base/shaders/ray_tracing/post.vert.spv"));
            graphic_shader_modules[Graphic_Pipeline::Main_FRAG].reset(
                    new ShaderModule("/home/mocheng/project/MCRT/example/base/shaders/ray_tracing/post.frag.spv"));

            PASS[Graphic]->prepare();
            PASS[Graphic]->prepare_descriptorset([&]() {


                PASS[Graphic]->get_descriptor_manager()->Make_DescriptorSet(
                        std::vector{get_rt_context()
                                            ->get_out_image()},
                        DescriptorManager::Graphic,
                        1,
                        vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment);
            });
            PASS[Graphic]->prepare_pipeline(graphic_shader_modules,
                                            {PASS[Graphic]->get_descriptor_manager()->get_DescriptorSet(
                                                    DescriptorManager::Graphic)}, sizeof(PushContant));

            PASS[Graphic]->post_prepare();
        }
//        IBLManager::Get_Singleton()->pre_compute_LUT();
//        IBLManager::Get_Singleton()->pre_compute_irradiance();
    }

    void cloud_context::re_create_context() {
        get_rt_context()->re_create();
        //

        auto rt_pass = get_rt_context();

        //    Descriptor_Manager::Get_Singleton()
        //        ->Make_DescriptorSet(std::vector { rt_context->get_out_image() },
        //                             Descriptor_Manager::Ray_Tracing,
        //                             (int)Ray_Tracing_Binding::e_out_image,
        //                             vk::DescriptorType::eStorageImage,
        //                             vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
        //    Descriptor_Manager::Get_Singleton()->update_descriptor_set(Descriptor_Manager::Ray_Tracing);
        //
        //    // get_graphic_context()->re_create();
        //    Descriptor_Manager::Get_Singleton()
        //        ->Make_DescriptorSet(
        //            std::vector {
        //                Context::Get_Singleton()
        //                    ->get_rt_context()
        //                    ->get_out_image() },
        //            Descriptor_Manager::Graphic,
        //            1,
        //            vk::DescriptorType::eCombinedImageSampler,
        //            vk::ShaderStageFlagBits::eFragment);
        //    Descriptor_Manager::Get_Singleton()
        //        ->update_descriptor_set(Descriptor_Manager::Graphic);

        ///
        rt_pass->get_descriptor_manager()
                ->Make_DescriptorSet(std::vector{rt_pass->get_out_image()},
                                     DescriptorManager::Ray_Tracing,
                                     (int) Ray_Tracing_Binding::e_out_image,
                                     vk::DescriptorType::eStorageImage,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
        rt_pass->get_descriptor_manager()->update_descriptor_set(DescriptorManager::Ray_Tracing);

        get_graphic_context()->get_descriptor_manager()
                ->Make_DescriptorSet(
                        std::vector{
                                get_rt_context()
                                        ->get_out_image()},
                        DescriptorManager::Graphic,
                        1,
                        vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment);
        get_graphic_context()->get_descriptor_manager()
                ->update_descriptor_set(DescriptorManager::Graphic);
    }

    std::shared_ptr<CommandBuffer> cloud_context::Begin_Frame() {

        BeginRTFrame();
        EndRTFrame();

        return ray_tracing_context::Begin_Frame();
    }

    void cloud_context::EndFrame() {

        ray_tracing_context::EndFrame();
        // std::cout << use_normal_map << std::endl;
        AS_Builder::Get_Singleton()->build_blas(true);
        AS_Builder::Get_Singleton()->build_tlas(true);
    }

    std::shared_ptr<CommandBuffer> cloud_context::BeginRTFrame() {
        // get_device()->get_handle().waitIdle();
        auto &rt_context = PASS[Ray_tracing];
        auto cmd = rt_context->BeginFrame();
        {

            cmd->get_handle()
                    .bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, rt_context->get_pipeline()->get_handle());
            std::vector<vk::DescriptorSet> descriptor_sets{
                    rt_context->get_pipeline()->get_descriptor_sets()

            };

            cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eRayTracingKHR,
                                                 rt_context->get_pipeline()->get_layout(),
                                                 0,
                                                 descriptor_sets,
                                                 {});
            auto camero_pos{glm::vec4{m_camera->m_position, 0}};
            auto cur_time = std::chrono::system_clock::now().time_since_epoch();
            auto ee = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time).count();
//            std::cout<<ee<<std::endl;
            pushContant_Ray = PushContant_Cloud{


                    .lightPosition{light_pos_x, light_pos_y, light_pos_z, 0},

                    .frame = frame_id,
                    .offset = offset++,
                    .noise_scale = 1.f / (5.f * noise_scale),
                    .thickness = thickness,
                    .speed = speed
                    // .clearColor { 1 },

            };
            frame_id++;


            cmd->get_handle()
                    .pushConstants<PushContant_Cloud>(rt_context->get_pipeline()
                                                              ->get_layout(),
                                                      vk::ShaderStageFlagBits::eRaygenKHR |
                                                      vk::ShaderStageFlagBits::eClosestHitKHR |
                                                      vk::ShaderStageFlagBits::eMissKHR,
                                                      0,
                                                      pushContant_Ray);
            cmd->get_handle().pipelineBarrier2(
                    vk::DependencyInfo()
                            .setMemoryBarriers(
                                    vk::MemoryBarrier2()
                                            .setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR)
                                            .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
                                            .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
                                            .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead)));
            rt_context->record_command(cmd);
        }

        return cmd;
    }

    void cloud_context::EndRTFrame() {
        // auto& rt_context = PASS[Ray_tracing];
        auto rt_context = get_rt_context();

        rt_context->Submit();
        rt_context->EndFrame();
    }

    std::shared_ptr<CommandBuffer> cloud_context::BeginComputeFrame() {
        throw std::runtime_error("no compute frame");
    }

    void cloud_context::EndComputeFrame() {
    }
}
