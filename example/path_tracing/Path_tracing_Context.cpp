
#include "example/path_tracing/Path_tracing_Context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/Texture.hpp"
#include "iostream"
#include "shader/Data_struct.h"

namespace MCRT {
std::unique_ptr<Context> Context::_instance {
    new MCRT::Path_tracing_context
};
Path_tracing_context::Path_tracing_context()
{
}
Path_tracing_context::~Path_tracing_context()
{
}
void Path_tracing_context::prepare(std::shared_ptr<Window> window)
{
    ray_tracing_context::prepare(window);
    GLTF_Loader::load_model("D:/MoChengRT/assets/cube.glb");
    auto mm = Mesh::meshs;
    auto tt = Texture::textures;
    // return;

    contexts.resize(3);
    // raytracing
    {

        contexts[Context_index::Ray_tracing] = std::shared_ptr<RT_Context> { new RT_Context(m_device) };
        std::vector<std::shared_ptr<ShaderModule>> rt_shader_modules(RT_Pipeline::eShaderGroupCount);
        rt_shader_modules[RT_Pipeline::eRaygen].reset(new ShaderModule("D:/MoChengRT/shader/Path_tracing/path_tracing.rgen.spv"));
        rt_shader_modules[RT_Pipeline::eMiss].reset(new ShaderModule("D:/MoChengRT/shader/Path_tracing/path_tracing.rmiss.spv"));
        rt_shader_modules[RT_Pipeline::eClosestHit].reset(new ShaderModule("D:/MoChengRT/shader/Path_tracing/path_tracing.rchit.spv"));
        Context::Get_Singleton()->get_rt_context()->set_hit_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_miss_shader_count(1);
        Context::Get_Singleton()->get_rt_context()->set_constants_size(sizeof(PushContant));

        contexts[Ray_tracing]->prepare(rt_shader_modules);
        contexts[Ray_tracing]->prepare_descriptorset([&]() {
            auto rt_context = Context::Get_Singleton()->get_rt_context();
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(AS_Builder::Get_Singleton()->get_tlas(),
                                     Descriptor_Manager::Ray_Tracing,
                                     Ray_Tracing_Binding::e_tlas,
                                     vk::DescriptorType::eAccelerationStructureKHR,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR);

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(rt_context->get_out_image(),
                                     Descriptor_Manager::Ray_Tracing,
                                     Ray_Tracing_Binding::e_out_image,
                                     vk::DescriptorType::eStorageImage,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(Texture::get_image_handles(),
                                     Descriptor_Manager::Global,
                                     Global_Binding::eTextures,
                                     vk::DescriptorType::eCombinedImageSampler,
                                     vk::ShaderStageFlagBits::eClosestHitKHR);
            // GBuffer

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(rt_context->get_gbuffer(),
                                     Descriptor_Manager::Ray_Tracing,
                                     Ray_Tracing_Binding::e_gbuffer,
                                     vk::DescriptorType ::eStorageImage,
                                     vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eCompute);
        });
        contexts[Ray_tracing]->prepare_pipeline(rt_shader_modules);

        contexts[Ray_tracing]->post_prepare();
    }
    {
        // Compute_Context
        contexts[Context_index::Compute] = std::shared_ptr<Compute_Context> { new Compute_Context };

        std::shared_ptr<ShaderModule> compute_shader {
            new ShaderModule("D:/MoChengRT/shader/filter.comp.spv")
        };
        contexts[Compute]->prepare({ compute_shader });
        contexts[Compute]->prepare_descriptorset([&]() {
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(Context::Get_Singleton()
                                         ->get_compute_context()
                                         ->get_out_image(),
                                     Descriptor_Manager::Compute,
                                     0,
                                     vk::DescriptorType::eStorageImage,
                                     vk::ShaderStageFlagBits::eCompute);
        });
        contexts[Compute]->prepare_pipeline({ compute_shader });
        contexts[Compute]->post_prepare();
    }
    { // graphic
        contexts[Context_index::Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };

        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::VERT].reset(new ShaderModule("D:/MoChengRT/shader/post.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::FRAG].reset(new ShaderModule("D:/MoChengRT/shader/post.frag.spv"));
        contexts[Graphic]->prepare(graphic_shader_modules);
        contexts[Graphic]->prepare_descriptorset([&]() { Descriptor_Manager::Get_Singleton()
                                                             ->Make_DescriptorSet(
                                                                 Context::Get_Singleton()
                                                                     ->get_compute_context()
                                                                     ->get_out_image(),
                                                                 Descriptor_Manager::Graphic,
                                                                 0,
                                                                 vk::DescriptorType ::eCombinedImageSampler,
                                                                 vk::ShaderStageFlagBits::eFragment); });
        contexts[Graphic]->prepare_pipeline(graphic_shader_modules);
        contexts[Graphic]->post_prepare();
    }
}
std::shared_ptr<CommandBuffer> Path_tracing_context::Begin_Frame()
{

    BeginRTFrame();
    EndRTFrame();
    BeginComputeFrame();
    EndComputeFrame();

    return ray_tracing_context::Begin_Frame();
}
void Path_tracing_context::EndFrame()
{
    ray_tracing_context::EndFrame();
}
std::shared_ptr<CommandBuffer> Path_tracing_context::BeginRTFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& rt_context = contexts[Ray_tracing];
    auto cmd = rt_context->BeginFrame();
    {

        cmd->get_handle()
            .bindPipeline(vk::PipelineBindPoint ::eRayTracingKHR, rt_context->get_pipeline()->get_handle());

        cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint ::eRayTracingKHR,
                                             rt_context->get_pipeline()->get_layout(),
                                             0,
                                             rt_context->get_pipeline()->get_descriptor_sets(),
                                             {});

        pushContant_Ray = PushContant {
            .frame = frame_id,
            // .clearColor { 1 },
            .lightPosition { 10.f, 15.f, 8.f, 0 },
            .lightIntensity = 100
        };
        frame_id++;
        cmd->get_handle()
            .pushConstants<PushContant>(rt_context->get_pipeline()
                                            ->get_layout(),
                                        vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR | vk::ShaderStageFlagBits::eMissKHR,
                                        0,
                                        pushContant_Ray);
        rt_context->record_command(cmd);
    }
    return cmd;
}

void Path_tracing_context::EndRTFrame()
{
    auto& rt_context = contexts[Ray_tracing];
    rt_context->Submit();
}

std::shared_ptr<CommandBuffer> Path_tracing_context::BeginComputeFrame()
{

    // compute_context->record_command(cmd);
    auto& compute_context = contexts[Compute];
    std::shared_ptr<CommandBuffer> cmd = compute_context->BeginFrame();
    {
        cmd->get_handle()
            .bindDescriptorSets(vk::PipelineBindPoint ::eCompute,
                                compute_context->get_pipeline()->get_layout(),
                                0,
                                compute_context->get_pipeline()->get_descriptor_sets(),
                                {});
        cmd->get_handle()
            .bindPipeline(vk::PipelineBindPoint::eCompute,
                          compute_context->get_pipeline()->get_handle());

        vk::MemoryBarrier2 memory_barrier, memory_barrier2, memory_barrier3, memory_barrier4;
        memory_barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR)
            .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
            .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead);
        // memory_barrier2.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        //     .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
        //     .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
        //     .setDstAccessMask(vk::AccessFlagBits2::eShaderWrite);
        memory_barrier3.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
            .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead);
        memory_barrier4.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
            .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
            .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
            .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead | vk::AccessFlagBits2::eShaderSampledRead);
        std::vector<vk::MemoryBarrier2> barriers {
            memory_barrier,
            // memory_barrier2,
            memory_barrier3,
            memory_barrier4
        };
        cmd->get_handle()
            .dispatch(800, 749, 1);
        auto pushConstantsss = Context::Get_Singleton()->get_enable_filter();
        auto push_contants = PushContant_Compute {
            .frame = Context::Get_Singleton()->get_cur_frame_id(),
            .open_filter = Context::Get_Singleton()->get_enable_filter()
        };
        cmd->get_handle()
            .pushConstants<PushContant_Compute>(compute_context->get_pipeline()->get_layout(), vk::ShaderStageFlagBits::eCompute, 0, push_contants);

        Context::Get_Singleton()->get_debugger()->set_name(cmd, "compute command_buffer");

        cmd->get_handle()
            .pipelineBarrier2(vk::DependencyInfo().setMemoryBarriers(barriers));

        compute_context->record_command(cmd);
    }
    return cmd;
}
void Path_tracing_context::EndComputeFrame()
{
    auto& compute_context = contexts[2];
    compute_context->Submit();
}
}