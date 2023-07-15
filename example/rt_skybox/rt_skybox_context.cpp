
#include "example/rt_skybox/rt_skybox_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/Skybox.hpp"
#include "iostream"
#include "shader/RT_skybox/Binding.h"
#include "shader/RT_skybox/Push_Constants.h"
namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::rt_skybox_context };
float rt_skybox_context::light_pos_x = 0, rt_skybox_context::light_pos_y = 0, rt_skybox_context::light_pos_z = 5;
rt_skybox_context::rt_skybox_context()
{
}
rt_skybox_context::~rt_skybox_context()
{
}
void rt_skybox_context::prepare(std::shared_ptr<Window> window)
{
    ray_tracing_context::prepare(window);
    m_skybox.reset(new Skybox("D:/MoChengRT/assets/Cubemap/farm"));
    skybox_mesh = GLTF_Loader::load_skybox("D:/MoChengRT/assets/cube.gltf");
    GLTF_Loader::load_model("D:/MoChengRT/assets/sphere.gltf");

    contexts.resize(2);
    // raytracing
    {

        contexts[Ray_tracing] = std::shared_ptr<RT_Context> { new RT_Context(m_device) };
        std::vector<std::shared_ptr<ShaderModule>> rt_shader_modules(RT_Pipeline::eShaderGroupCount);
        rt_shader_modules[RT_Pipeline::eRaygen].reset(new ShaderModule("D:/MoChengRT/shader/RT_skybox/skybox.rgen.spv"));
        rt_shader_modules[RT_Pipeline::eMiss].reset(new ShaderModule("D:/MoChengRT/shader/RT_skybox/skybox.rmiss.spv"));
        // rt_shader_modules[RT_Pipeline::eMiss2].reset(new ShaderModule("D:/MoChengRT/shader/RT_skybox/skybox_shadow.rmiss.spv"));
        rt_shader_modules[RT_Pipeline::eClosestHit].reset(new ShaderModule("D:/MoChengRT/shader/RT_skybox/skybox.rchit.spv"));
        Context::Get_Singleton()->get_rt_context()->set_hit_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_miss_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_constants_size(sizeof(PushContant));
        contexts[Ray_tracing]->prepare();
        contexts[Ray_tracing]->prepare_descriptorset([&]() {
            auto rt_context = Context::Get_Singleton()->get_rt_context();
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(std::vector { AS_Builder::Get_Singleton()->get_tlas() },
                                     Descriptor_Manager::Ray_Tracing,
                                     Ray_Tracing_Binding::e_tlas,
                                     vk::DescriptorType::eAccelerationStructureKHR,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR);

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(std::vector { rt_context->get_out_image() },
                                     Descriptor_Manager::Ray_Tracing,
                                     Ray_Tracing_Binding::e_out_image,
                                     vk::DescriptorType::eStorageImage,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(std::vector { m_skybox->get_handle() },
                                     Descriptor_Manager::Ray_Tracing,
                                     Ray_Tracing_Binding::e_rt_skybox,
                                     vk::DescriptorType::eStorageImage,
                                     vk::ShaderStageFlagBits::eMissKHR);
        });
        contexts[Ray_tracing]->prepare_pipeline(rt_shader_modules);

        contexts[Ray_tracing]->post_prepare();
    }

    { // graphic
        contexts[Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };
        contexts[Graphic]->set_constants_size(sizeof(PushContant));
        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::VERT].reset(new ShaderModule("D:/MoChengRT/shader/post.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::FRAG].reset(new ShaderModule("D:/MoChengRT/shader/post.frag.spv"));
        contexts[Graphic]->prepare();
        contexts[Graphic]->prepare_descriptorset([&]() { Descriptor_Manager::Get_Singleton()
                                                             ->Make_DescriptorSet(
                                                                 std::vector { Context::Get_Singleton()
                                                                                   ->get_rt_context()
                                                                                   ->get_out_image() },
                                                                 Descriptor_Manager::Graphic,
                                                                 0,
                                                                 vk::DescriptorType ::eCombinedImageSampler,
                                                                 vk::ShaderStageFlagBits::eFragment); });
        contexts[Graphic]->prepare_pipeline(graphic_shader_modules);

        contexts[Graphic]->post_prepare();
    }
}
std::shared_ptr<CommandBuffer> rt_skybox_context::Begin_Frame()
{
    BeginRTFrame();
    EndRTFrame();

    return ray_tracing_context::Begin_Frame();
}
void rt_skybox_context::EndFrame()
{

    ray_tracing_context::EndFrame();
}

std::shared_ptr<CommandBuffer> rt_skybox_context::BeginRTFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& rt_context = contexts[Ray_tracing];
    auto cmd = rt_context->BeginFrame();
    {

        cmd->get_handle()
            .bindPipeline(vk::PipelineBindPoint ::eRayTracingKHR, rt_context->get_pipeline()->get_handle());
        std::vector<vk::DescriptorSet> descriptor_sets {
            // descriptor_sets.push_back(
            Descriptor_Manager::Get_Singleton()
                ->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)
                ->get_handle()[0],

            Descriptor_Manager::Get_Singleton()
                ->get_DescriptorSet(Descriptor_Manager::Global)
                ->get_handle()[0]
        };

        cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint ::eRayTracingKHR,
                                             rt_context->get_pipeline()->get_layout(),
                                             0,
                                             descriptor_sets,
                                             {});

        pushContant_Ray = PushContant {
            // .view_inverse = glm::inverse(m_camera->Get_v_matrix()),
            // .project_inverse = glm::inverse(m_camera->Get_p_matrix()),
            .frame = frame_id,
            // .clearColor { 1 },
            .lightPosition { light_pos_x, light_pos_y, light_pos_z, 0 },
            .lightIntensity = 10
        };
        frame_id++;
        cmd->get_handle()
            .pushConstants<PushContant>(rt_context->get_pipeline()
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

void rt_skybox_context::EndRTFrame()
{
    auto& rt_context = contexts[Ray_tracing];
    rt_context->Submit();
}

std::shared_ptr<CommandBuffer> rt_skybox_context::BeginComputeFrame()
{
    throw std::runtime_error("no compute frame");
}
void rt_skybox_context::EndComputeFrame()
{
}
}