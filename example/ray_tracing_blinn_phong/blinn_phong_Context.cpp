
#include "example/ray_tracing_blinn_phong/blinn_phong_Context.hpp"
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
#include "iostream"
#include "shader/Blinn_Phong/Binding.h"
#include "shader/Blinn_Phong/Push_Constants.h"
namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::blinn_phong_context };
float blinn_phong_context::light_pos_x = 0, blinn_phong_context::light_pos_y = 0, blinn_phong_context::light_pos_z = 5;
blinn_phong_context::blinn_phong_context()
{
}
blinn_phong_context::~blinn_phong_context()
{
}
void blinn_phong_context::prepare(std::shared_ptr<Window> window)
{
    ray_tracing_context::prepare(window);
    // m_camera->m_position.z = 20.f;
    // Obj_loader::load_model("D:/MoChengRT/assets/no.obj");
    GLTF_Loader::load_model("D:/MoChengRT/assets/cube.glb");

    contexts.resize(2);
    // raytracing
    {

        contexts[Ray_tracing] = std::shared_ptr<RT_Context> { new RT_Context(m_device) };
        std::vector<std::shared_ptr<ShaderModule>> rt_shader_modules(RT_Pipeline::eShaderGroupCount);
        rt_shader_modules[RT_Pipeline::eRaygen].reset(new ShaderModule("D:/MoChengRT/shader/Blinn_Phong/blinn_phong.rgen.spv"));
        rt_shader_modules[RT_Pipeline::eMiss].reset(new ShaderModule("D:/MoChengRT/shader/Blinn_Phong/blinn_phong.rmiss.spv"));
        rt_shader_modules[RT_Pipeline::eMiss2].reset(new ShaderModule("D:/MoChengRT/shader/Blinn_Phong/blinn_phong_shadow.rmiss.spv"));
        rt_shader_modules[RT_Pipeline::eClosestHit].reset(new ShaderModule("D:/MoChengRT/shader/Blinn_Phong/blinn_phong.rchit.spv"));
        Context::Get_Singleton()->get_rt_context()->set_hit_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_miss_shader_count(2);
        Context::Get_Singleton()->get_rt_context()->set_constants_size(sizeof(PushContant));
        contexts[Ray_tracing]->prepare(rt_shader_modules);
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
        contexts[Graphic]->prepare(graphic_shader_modules);
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
std::shared_ptr<CommandBuffer> blinn_phong_context::Begin_Frame()
{
    BeginRTFrame();
    EndRTFrame();

    return ray_tracing_context::Begin_Frame();
}
void blinn_phong_context::EndFrame()
{

    ray_tracing_context::EndFrame();
}

std::shared_ptr<CommandBuffer> blinn_phong_context::BeginRTFrame()
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

void blinn_phong_context::EndRTFrame()
{
    auto& rt_context = contexts[Ray_tracing];
    rt_context->Submit();
}

std::shared_ptr<CommandBuffer> blinn_phong_context::BeginComputeFrame()
{
    throw std::runtime_error("no compute frame");
}
void blinn_phong_context::EndComputeFrame()
{
}
}