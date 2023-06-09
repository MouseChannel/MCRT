
#include "example/raster/raster_pbr_context.hpp"
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
#include "Wrapper/Skybox.hpp"
#include "Wrapper/Texture.hpp"
#include "iostream"

// #include "shader/Data_struct.h"
// #include "shader/Raster/Constants.h"

#include "shader/Raster/Binding.h"

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::raster_context_pbr };
float raster_context_pbr::light_pos_x = 0, raster_context_pbr::light_pos_y = 0, raster_context_pbr::light_pos_z = 5;
raster_context_pbr::raster_context_pbr()
{
}
raster_context_pbr::~raster_context_pbr()
{
}
void raster_context_pbr::prepare(std::shared_ptr<Window> window)
{
    raster_context::prepare(window);
    sky_box.reset(new Skybox("D:/MoChengRT/assets/Cubemap/Hotel"));

    // Obj_loader::load_model("D:/MoChengRT/assets/girl.obj");
    GLTF_Loader::load_model("D:/MoChengRT/assets/cube.gltf");
    auto dd = Texture::textures;
    auto ss = Mesh::meshs;

    contexts.resize(1);

    { // graphic
        contexts[Context_index::Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };
        Context::Get_Singleton()
            ->get_graphic_context()
            ->set_constants_size(sizeof(PC_Raster));
        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::VERT].reset(new ShaderModule("D:/MoChengRT/shader/raster/skybox.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::FRAG].reset(new ShaderModule("D:/MoChengRT/shader/raster/skybox.frag.spv"));
        contexts[Graphic]->prepare(graphic_shader_modules);
        contexts[Graphic]->prepare_descriptorset([&]() {
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    camera_matrix,
                    e_camera_matrix,
                    Descriptor_Manager::Graphic);
            // sky_box

            Descriptor_Manager::Get_Singleton()->Make_DescriptorSet(sky_box->get_handle(),
                                                                    Descriptor_Manager::Graphic,
                                                                    1,
                                                                    vk::DescriptorType ::eCombinedImageSampler,
                                                                    vk::ShaderStageFlagBits::eFragment);

            // Descriptor_Manager::Get_Singleton()
            //     ->Make_DescriptorSet(
            //         Texture::get_image_handles(),
            //         Descriptor_Manager::Graphic,
            //         e_textures,
            //         vk::DescriptorType ::eCombinedImageSampler,
            //         vk::ShaderStageFlagBits::eFragment);
        });
        contexts[Graphic]->prepare_pipeline(graphic_shader_modules);

        contexts[Graphic]->post_prepare();
    }
}
std::shared_ptr<CommandBuffer> raster_context_pbr::Begin_Frame()
{

    return raster_context::Begin_Frame();
}
void raster_context_pbr::EndFrame()
{
    raster_context::EndFrame();
}

std::shared_ptr<CommandBuffer> raster_context_pbr::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& render_context = contexts[Graphic];
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {

        cmd->get_handle()
            .bindPipeline(vk::PipelineBindPoint ::eGraphics,
                          render_context->get_pipeline()->get_handle());

        cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             render_context->get_pipeline()->get_layout(),
                                             0,
                                             { Descriptor_Manager::Get_Singleton()
                                                   ->get_DescriptorSet(Descriptor_Manager::Graphic)
                                                   ->get_handle() },
                                             {});
        // cmd->get_handle()
        //     .updateBuffer<Camera_matrix>(
        //         camera_matrix->buffer->get_handle(),
        //         0,
        //         Camera_matrix {
        //             //    .camera_pos { pos },
        //             .view {
        //                 Context::Get_Singleton()
        //                     ->get_camera()
        //                     ->Get_v_matrix() },

        //             .project {
        //                 Context::Get_Singleton()
        //                     ->get_camera()
        //                     ->Get_p_matrix() } });

        for (auto& mesh : Mesh::meshs) {

            cmd->get_handle().bindIndexBuffer(mesh->get_indices_buffer()->get_handle(),
                                              0,
                                              vk::IndexType ::eUint32);
            cmd->get_handle().bindVertexBuffers(0, {
                                                       mesh->get_vertex_buffer()->get_handle(),
                                                   },
                                                { 0 });
            auto res = Context::Get_Singleton()
                           ->get_camera()
                           ->Get_v_matrix();
            res[3] = { 0, 0, 0, 1 };
            pc = PC_Raster {
                // .model_matrix {
                //     glm::rotate(mesh->get_model_matrix(), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f))
                // },
                .model_matrix { res },

                .view_matrix { Context::Get_Singleton()
                                   ->get_camera()
                                   ->Get_v_matrix() },
                .texture_index = mesh->m_material.texture_index
            };
            // angle++;

            cmd->get_handle()
                .pushConstants<PC_Raster>(
                    render_context
                        ->get_pipeline()
                        ->get_layout(),
                    vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                    0,
                    pc);
            cmd->get_handle()
                .drawIndexed(mesh->get_vertex_count(),
                             1,
                             0,
                             0,
                             0);
        }
    }
    render_context->record_command(cmd);

    return cmd;
}

void raster_context_pbr::EndGraphicFrame()
{
    auto& m_render_context = contexts[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}
// std::shared_ptr<CommandBuffer> raster_context_pbr::BeginComputeFrame()
// {

//     // compute_context->record_command(cmd);
//     auto& compute_context = contexts[2];
//     std::shared_ptr<CommandBuffer> cmd = compute_context->BeginFrame();
//     {
//         cmd->get_handle()
//             .bindDescriptorSets(vk::PipelineBindPoint ::eCompute,
//                                 compute_context->get_pipeline()->get_layout(),
//                                 0,
//                                 { Descriptor_Manager::Get_Singleton()
//                                       ->get_DescriptorSet(Descriptor_Manager::Ray_Tracing)
//                                       ->get_handle()[0],
//                                   Descriptor_Manager::Get_Singleton()
//                                       ->get_DescriptorSet(Descriptor_Manager::Compute)
//                                       ->get_handle()[0] },
//                                 {});
//         cmd->get_handle()
//             .bindPipeline(vk::PipelineBindPoint::eCompute,
//                           compute_context->get_pipeline()->get_handle());

//         vk::MemoryBarrier2 memory_barrier, memory_barrier2, memory_barrier3, memory_barrier4;
//         memory_barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR)
//             .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
//             .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
//             .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead);
//         // memory_barrier2.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
//         //     .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
//         //     .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
//         //     .setDstAccessMask(vk::AccessFlagBits2::eShaderWrite);
//         memory_barrier3.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
//             .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
//             .setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader)
//             .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead);
//         memory_barrier4.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader)
//             .setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite)
//             .setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
//             .setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead | vk::AccessFlagBits2::eShaderSampledRead);
//         std::vector<vk::MemoryBarrier2> barriers {
//             memory_barrier,
//             // memory_barrier2,
//             memory_barrier3,
//             memory_barrier4
//         };
//         cmd->get_handle()
//             .dispatch(800, 749, 1);
//         auto pushConstantsss = Context::Get_Singleton()->get_enable_filter();
//         auto push_contants = PushContant_Compute {
//             .frame = Context::Get_Singleton()->get_cur_frame_id(),
//             .open_filter = Context::Get_Singleton()->get_enable_filter()
//         };
//         cmd->get_handle()
//             .pushConstants<PushContant_Compute>(compute_context->get_pipeline()->get_layout(), vk::ShaderStageFlagBits::eCompute, 0, push_contants);

//         Context::Get_Singleton()->get_debugger()->set_name(cmd, "compute command_buffer");
//         // auto ee = get_out_image();
//         // cmd->get_handle().setCheckpointNV(testcheck);
//         cmd->get_handle()
//             .pipelineBarrier2(vk::DependencyInfo().setMemoryBarriers(barriers));

//         compute_context->record_command(cmd);
//     }
//     return cmd;
// }
// void raster_context_pbr::EndComputeFrame()
// {
//     auto& compute_context = contexts[2];
//     compute_context->Submit();
// }
}