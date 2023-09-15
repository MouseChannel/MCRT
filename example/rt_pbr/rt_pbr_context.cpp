
#include "example/rt_pbr/rt_pbr_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Tool/stb_image.h"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/Skybox.hpp"
#include "Wrapper/Texture.hpp"
#include "example/base/shader/ray_tracing/Data_struct.h"
#include "example/rt_pbr/shader/Binding.h"
#include "example/rt_pbr/shader/Push_Constants.h"
#include "iostream"
#include "shader/PBR/IBL/binding.h"
// #include ""

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::rt_pbr_context };
float rt_pbr_context::light_pos_x = 0, rt_pbr_context::light_pos_y = 0, rt_pbr_context::light_pos_z = 5, rt_pbr_context::roughness = 0.1, rt_pbr_context ::met = 0.1;
int rt_pbr_context::apply_normal = 0;
bool rt_pbr_context::use_normal_map = false;
int irradiance_size = 512;
rt_pbr_context::rt_pbr_context()
{
}
rt_pbr_context::~rt_pbr_context()
{
}
void rt_pbr_context::prepare(std::shared_ptr<Window> window)
{
    ray_tracing_context::prepare(window);
    m_skybox.reset(new Skybox("D:/MoCheng/MoChengRT/assets/Cubemap/farm"));
    skybox_mesh = GLTF_Loader::load_skybox("D:/MoCheng/MoChengRT/assets/cube.gltf");
    // GLTF_Loader::load_model("D:/MoCheng/MoChengRT/assets/pbr/cardbox.gltf");

    GLTF_Loader::load_model("D:/MoCheng/MoChengRT/assets/pbr/cat/cat.glb");

    LUT.reset(new Image(1024,
                        1024,
                        vk::Format::eR32G32B32A32Sfloat,
                        vk::ImageType::e2D,
                        vk::ImageTiling::eOptimal,
                        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled,
                        vk::ImageAspectFlagBits::eColor,
                        vk::SampleCountFlagBits::e1));
    LUT->SetImageLayout(vk::ImageLayout::eGeneral,
                        vk::AccessFlagBits::eNone,
                        vk::AccessFlagBits::eNone,
                        vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::PipelineStageFlagBits::eBottomOfPipe);
    irradiance.reset(new Skybox(irradiance_size, irradiance_size));

    contexts.resize(3);

    { // raytracing

        contexts[Ray_tracing] = std::shared_ptr<RT_Context> { new RT_Context(m_device) };
        std::vector<std::shared_ptr<ShaderModule>> rt_shader_modules(RT_Pipeline::eShaderGroupCount);
        rt_shader_modules[RT_Pipeline::eRaygen].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/RT_pbr/shader/rt_pbr.rgen.spv"));
        rt_shader_modules[RT_Pipeline::eMiss].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/RT_pbr/shader/rt_pbr.rmiss.spv"));
        // rt_shader_modules[RT_Pipeline::eMiss2].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/RT_pbr/shader/rt_pbr_shadow.rmiss.spv"));
        rt_shader_modules[RT_Pipeline::eClosestHit].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/RT_pbr/shader/rt_pbr.rchit.spv"));
        Context::Get_Singleton()->get_rt_context()->set_hit_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_miss_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_constants_size(sizeof(PushContant_rtpbr));
        contexts[Ray_tracing]->prepare();
        contexts[Ray_tracing]->prepare_descriptorset([&]() {
            auto rt_context = Context::Get_Singleton()->get_rt_context();
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(std::vector { AS_Builder::Get_Singleton()->get_tlas() },
                                     Descriptor_Manager::Ray_Tracing,
                                     (int)Ray_Tracing_Binding::e_tlas,
                                     vk::DescriptorType::eAccelerationStructureKHR,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR);

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(std::vector { rt_context->get_out_image() },
                                     Descriptor_Manager::Ray_Tracing,
                                     (int)Ray_Tracing_Binding::e_out_image,
                                     vk::DescriptorType::eStorageImage,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(Texture::get_image_handles(),
                                     Descriptor_Manager::Global,
                                     (int)Global_Binding::eTextures,

                                     //  (int)Global_Binding::eTextures,
                                     vk::DescriptorType::eCombinedImageSampler,
                                     vk::ShaderStageFlagBits::eClosestHitKHR);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(std::vector { m_skybox->get_handle() },
                                     Descriptor_Manager::Ray_Tracing,
                                     (int)Ray_Tracing_Binding::e_rt_skybox,
                                     vk::DescriptorType::eCombinedImageSampler,
                                     vk::ShaderStageFlagBits::eMissKHR | vk::ShaderStageFlagBits::eClosestHitKHR);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { LUT },
                    Descriptor_Manager::Ray_Tracing,
                    (int)Ray_Tracing_Binding::e_rt_LUT_image,
                    vk::DescriptorType ::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eClosestHitKHR);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { irradiance->get_handle() },
                    Descriptor_Manager::Ray_Tracing,
                    (int)Ray_Tracing_Binding::e_rt_irradiance_image,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eClosestHitKHR);
        });
        auto sets = std::vector<std::shared_ptr<DescriptorSet>>((int)Ray_Tracing_Set::ray_tracing_count);
        sets[(int)Ray_Tracing_Set::e_ray_tracing] = Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Ray_Tracing);
        sets[(int)Ray_Tracing_Set::e_ray_global] = Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Global);
        contexts[Ray_tracing]
            ->prepare_pipeline(rt_shader_modules,
                               sets,
                               sizeof(PushContant_rtpbr));
        contexts[Ray_tracing]->post_prepare();
    }

    { // graphic
        contexts[Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };
        contexts[Graphic]->set_constants_size(sizeof(PushContant));

        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::Main_VERT].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/base/shader/ray_tracing/post.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Main_FRAG].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/base/shader/ray_tracing/post.frag.spv"));

        contexts[Graphic]->prepare();
        contexts[Graphic]->prepare_descriptorset([&]() {
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { Context::Get_Singleton()
                                      ->get_rt_context()
                                      ->get_out_image() },
                    Descriptor_Manager::Graphic,
                    1,
                    vk::DescriptorType ::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
        });
        contexts[Graphic]->prepare_pipeline(graphic_shader_modules, { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Graphic) }, sizeof(PushContant));

        contexts[Graphic]->post_prepare();
    }
    { // compute_precompute

        contexts[Compute].reset(new Compute_Context);
        contexts[Compute]->set_constants_size(sizeof(PushContant_IBL));

        contexts[Compute]->prepare();
        contexts[Compute]->prepare_descriptorset([&]() {
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { irradiance->get_handle() },
                    Descriptor_Manager::Compute,
                    IBL_Binding::e_irradiance_image,
                    vk::DescriptorType ::eStorageImage,
                    vk::ShaderStageFlagBits::eCompute);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { LUT },
                    Descriptor_Manager::Compute,
                    IBL_Binding::e_LUT_image,
                    vk::DescriptorType ::eStorageImage,
                    vk::ShaderStageFlagBits::eCompute);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { m_skybox->get_handle() },
                    Descriptor_Manager::Compute,
                    IBL_Binding::e_skybox,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eCompute);
        });

        { // pre_compute_irradiance
            std::shared_ptr<ShaderModule>
                compute_shader {
                    new ShaderModule("D:/MoCheng/MoChengRT/shader/PBR/IBL/irradiance.comp.spv")
                };
            contexts[Compute]->prepare_pipeline({ compute_shader }, { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute) }, sizeof(PushContant_Compute));

            contexts[Compute]->post_prepare();
            // doing the actually work
            auto& compute_context = contexts[Compute];
            // std::shared_ptr<CommandBuffer> cmd = compute_context->BeginFrame();
            CommandManager::ExecuteCmd(m_device->Get_Graphic_queue(), [&](vk::CommandBuffer& cmd) {
                cmd.bindDescriptorSets(vk::PipelineBindPoint ::eCompute,
                                       compute_context->get_pipeline()->get_layout(),
                                       0,
                                       compute_context->get_pipeline()->get_descriptor_sets(),
                                       {});
                cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                                 compute_context->get_pipeline()->get_handle());

                cmd.dispatch(irradiance_size, irradiance_size, 6);

                // auto push_contants = PushContant_IBL {
                //     .doing_filter = 0
                // };
                // cmd.pushConstants<PushContant_IBL>(compute_context->get_pipeline()->get_layout(),
                //                                    vk::ShaderStageFlagBits::eCompute,
                //                                    0,
                //                                    push_contants);
            });
        }
        { // pre_compute_lut

            std::shared_ptr<ShaderModule>
                compute_shader {
                    new ShaderModule("D:/MoCheng/MoChengRT/shader/PBR/IBL/lookup_table.comp.spv")
                };

            contexts[Compute]->prepare_pipeline({ compute_shader }, { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute) }, sizeof(PushContant_Compute));

            contexts[Compute]->post_prepare();
            // doing the actually work
            auto& compute_context = contexts[Compute];

            CommandManager::ExecuteCmd(m_device->Get_Graphic_queue(), [&](vk::CommandBuffer& cmd) {
                cmd.bindDescriptorSets(vk::PipelineBindPoint ::eCompute,
                                       compute_context->get_pipeline()->get_layout(),
                                       0,
                                       compute_context->get_pipeline()->get_descriptor_sets(),
                                       {});
                cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                                 compute_context->get_pipeline()->get_handle());

                cmd.dispatch(1024, 1024, 1);
            });
        }
    }
}

void rt_pbr_context::re_create_context()
{
    get_rt_context()->re_create();
    //

    auto rt_context = get_rt_context();

    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(std::vector { rt_context->get_out_image() },
                             Descriptor_Manager::Ray_Tracing,
                             (int)Ray_Tracing_Binding::e_out_image,
                             vk::DescriptorType::eStorageImage,
                             vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
    Descriptor_Manager::Get_Singleton()->update_descriptor_set(Descriptor_Manager::Ray_Tracing);

    // get_graphic_context()->re_create();
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(
            std::vector {
                Context::Get_Singleton()
                    ->get_rt_context()
                    ->get_out_image() },
            Descriptor_Manager::Graphic,
            1,
            vk::DescriptorType ::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment);
    Descriptor_Manager::Get_Singleton()
        ->update_descriptor_set(Descriptor_Manager::Graphic);
}
std::shared_ptr<CommandBuffer> rt_pbr_context::Begin_Frame()
{
    BeginRTFrame();
    EndRTFrame();

    return ray_tracing_context::Begin_Frame();
}
void rt_pbr_context::EndFrame()
{

    ray_tracing_context::EndFrame();
    // std::cout << use_normal_map << std::endl;
    AS_Builder::Get_Singleton()->build_blas(true);
    AS_Builder::Get_Singleton()->build_tlas(true);
}

std::shared_ptr<CommandBuffer> rt_pbr_context::BeginRTFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& rt_context = contexts[Ray_tracing];
    auto cmd = rt_context->BeginFrame();
    {

        cmd->get_handle()
            .bindPipeline(vk::PipelineBindPoint ::eRayTracingKHR, rt_context->get_pipeline()->get_handle());
        std::vector<vk::DescriptorSet> descriptor_sets {
            rt_context->get_pipeline()->get_descriptor_sets()

        };

        cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint ::eRayTracingKHR,
                                             rt_context->get_pipeline()->get_layout(),
                                             0,
                                             descriptor_sets,
                                             {});
        auto camero_pos { glm::vec4 { m_camera->m_position, 0 } };
        pushContant_Ray = PushContant_rtpbr {

            .apply_normal = apply_normal,
            .use_normal_map = use_normal_map,
            .camera_pos { camero_pos },
            .lightPosition { light_pos_x, light_pos_y, light_pos_z, 0 },
            .roughness = roughness,
            .metallicness = met,
            .frame = frame_id,

            // .clearColor { 1 },

        };
        frame_id++;

        cmd->get_handle()
            .pushConstants<PushContant_rtpbr>(rt_context->get_pipeline()
                                                  ->get_layout(),
                                              vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR | vk::ShaderStageFlagBits::eMissKHR,
                                              0,
                                              pushContant_Ray);
        cmd->get_handle().pipelineBarrier2(vk::DependencyInfo()
                                               .setMemoryBarriers(vk::MemoryBarrier2()
                                                                      .setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR)
                                                                      .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
                                                                      .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
                                                                      .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead)));
        rt_context->record_command(cmd);
    }

    return cmd;
}

void rt_pbr_context::EndRTFrame()
{
    // auto& rt_context = contexts[Ray_tracing];
    auto rt_context = get_rt_context();

    rt_context->Submit();
    rt_context->EndFrame();
}

std::shared_ptr<CommandBuffer> rt_pbr_context::BeginComputeFrame()
{
    throw std::runtime_error("no compute frame");
}
void rt_pbr_context::EndComputeFrame()
{
}
}