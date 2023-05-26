
#include "example/ray_tracing_blinn_phong/blinn_phong_Context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
#include "iostream"
#include "shader/Data_struct.h"

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::blinn_phong_context };
float blinn_phong_context::light_pos_x = 0, blinn_phong_context::light_pos_y = 0, blinn_phong_context::light_pos_z = 5;
blinn_phong_context::blinn_phong_context()
{
}
blinn_phong_context::~blinn_phong_context()
{
}
void blinn_phong_context::prepare()
{
    std::vector<uint32_t> indices {
        0,
        1,
        2,
        0,
        1,
        3

    };
    std::vector<float> positions {
        1.0f,
        -1.0f,
        0,

        -1.0f,
        1.0f,
        0,

        -1.0f,
        -1.0f,
        0,

        1.0f,
        1.0f,
        0

    };
    std::vector<float> uvs {
        1.0f,
        1.0f,

        0.0f,
        0.0f,

        0.0f,
        1.0f,

        1.0f,
        0.0f
    };
    index_buffer = Buffer::CreateDeviceBuffer(indices.data(), indices.size() * sizeof(indices[0]), vk::BufferUsageFlagBits::eIndexBuffer);
    vertex_buffer = Buffer::CreateDeviceBuffer(positions.data(), positions.size() * sizeof(positions[0]), vk::BufferUsageFlagBits::eVertexBuffer);
    uv_buffer = Buffer::CreateDeviceBuffer(uvs.data(), uvs.size() * sizeof(uvs[0]), vk::BufferUsageFlagBits::eVertexBuffer);
}
void blinn_phong_context::init(std::shared_ptr<Window> window)
{
    Context::init(window);
    prepare();
    Obj_loader::load_model("D:/MoChengRT/assets/girl.obj");

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
    m_camera->move_update();
    auto cmd = BeginRTFrame();
    EndRTFrame();
    // BeginComputeFrame();
    // EndComputeFrame();

    return BeginGraphicFrame();
}
void blinn_phong_context::EndFrame()
{

    EndGraphicFrame();
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
std::shared_ptr<CommandBuffer> blinn_phong_context::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& render_context = contexts[Graphic];
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        cmd->get_handle().bindPipeline(vk::PipelineBindPoint ::eGraphics, render_context->get_pipeline()->get_handle());
        cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             render_context->get_pipeline()->get_layout(),
                                             0,
                                             { //   Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute)->get_handle()[0],
                                               Descriptor_Manager::Get_Singleton()
                                                   ->get_DescriptorSet(Descriptor_Manager::Graphic)
                                                   ->get_handle() },

                                             {});
        cmd->get_handle().bindIndexBuffer(index_buffer->get_handle(), 0, vk::IndexType ::eUint32);
        cmd->get_handle().bindVertexBuffers(0, {
                                                   vertex_buffer->get_handle(),
                                                   uv_buffer->get_handle(),
                                               },
                                            { 0, 0 });
        cmd->get_handle()
            .drawIndexed(6, 1, 0, 0, 0);
        render_context->record_command(cmd);
    }

    return cmd;
}

void blinn_phong_context::EndGraphicFrame()
{
    auto& m_render_context = contexts[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}
std::shared_ptr<CommandBuffer> blinn_phong_context::BeginComputeFrame()
{

    // compute_context->record_command(cmd);
    auto& compute_context = contexts[2];
    std::shared_ptr<CommandBuffer> cmd = compute_context->BeginFrame();
    {
        cmd->get_handle()
            .bindDescriptorSets(vk::PipelineBindPoint ::eCompute,
                                compute_context->get_pipeline()->get_layout(),
                                0,
                                { Descriptor_Manager::Get_Singleton()
                                      ->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)
                                      ->get_handle()[0],
                                  Descriptor_Manager::Get_Singleton()
                                      ->get_DescriptorSet(Descriptor_Manager::Compute)
                                      ->get_handle()[0] },
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
        // auto ee = get_out_image();
        // cmd->get_handle().setCheckpointNV(testcheck);
        cmd->get_handle()
            .pipelineBarrier2(vk::DependencyInfo().setMemoryBarriers(barriers));

        compute_context->record_command(cmd);
    }
    return cmd;
}
void blinn_phong_context::EndComputeFrame()
{
    auto& compute_context = contexts[2];
    compute_context->Submit();
}
}