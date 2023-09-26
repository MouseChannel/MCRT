
#include "example/raster/raster_pbr_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
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

#include "shaders/PBR/IBL/binding.h"

#include "example/raster/shader/Binding.h"
#include "shaders/PBR/IBL/push_constants.h"

#include "example/raster/shader/Constants.h"

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::raster_context_pbr };
float raster_context_pbr::light_pos_x = 0, raster_context_pbr::light_pos_y = 0, raster_context_pbr::light_pos_z = 5;
bool raster_context_pbr::use_normal_map = false, raster_context_pbr::use_r_rm_map = false;
int irradiance_size = 512;
raster_context_pbr::raster_context_pbr()
{
}
raster_context_pbr::~raster_context_pbr()
{
}
void raster_context_pbr::prepare(std::shared_ptr<Window> window)
{
    raster_context::prepare(window);
    m_camera->m_position.z = 3;
    sky_box.reset(new Skybox("D:/MoCheng/MoChengRT/assets/Cubemap/farm"));
    skybox_mesh = GLTF_Loader::load_skybox("D:/MoCheng/MoChengRT/assets/cube.gltf");

    GLTF_Loader::load_model("D:/MoCheng/MoChengRT/assets/box.gltf");
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

    contexts.resize(2);

    {
        contexts[Context_index::Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };
        Context::Get_Singleton()
            ->get_graphic_context()
            ->set_constants_size(sizeof(PC_Raster));
        auto graphic_context = std::reinterpret_pointer_cast<RenderContext>(contexts[Graphic]);
        if (graphic_context == nullptr) {
            throw std::runtime_error("not graphic context");
        }
        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::Main_VERT].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/raster/shader/raster.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Main_FRAG].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/raster/shader/raster.frag.spv"));
        graphic_shader_modules[Graphic_Pipeline::Skybox_VERT].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/raster/shader/skybox.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Skybox_FRAG].reset(new ShaderModule("D:/MoCheng/MoChengRT/example/raster/shader/skybox.frag.spv"));
        graphic_context->prepare();
        graphic_context->prepare_descriptorset([&]() {
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    camera_matrix,
                    (int)Graphic_Binding::e_camera_matrix,
                    Descriptor_Manager::Graphic);

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    sky_box->get_handle(),
                    Descriptor_Manager::Graphic,
                    (int)Graphic_Binding::e_skybox,
                    vk::DescriptorType ::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);

            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    Texture::get_image_handles(),
                    Descriptor_Manager::Graphic,
                    (int)Graphic_Binding::e_textures,
                    vk::DescriptorType ::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { LUT },
                    Descriptor_Manager::Graphic,
                    (int)Graphic_Binding::e_LUT_image,
                    vk::DescriptorType ::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
            Descriptor_Manager::Get_Singleton()
                ->Make_DescriptorSet(
                    std::vector { irradiance->get_handle() },
                    Descriptor_Manager::Graphic,
                    (int)Graphic_Binding::e_irradiance_image,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
        });
        graphic_context->prepare_pipeline(graphic_shader_modules, { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Graphic) }, sizeof(PC_Raster));

        graphic_context->post_prepare();
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
                    std::vector { sky_box->get_handle() },
                    Descriptor_Manager::Compute,
                    IBL_Binding::e_skybox,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eCompute);
        });

        { // pre_compute_irradiance
            std::shared_ptr<ShaderModule>
                compute_shader {
                    new ShaderModule("D:/MoCheng/MoChengRT/shaders/PBR/IBL/irradiance.comp.spv")
                };
            contexts[Compute]->prepare_pipeline({ compute_shader },
                                                { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute) },
                                                sizeof(PushContant_Compute));

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
            });
        }
        { // pre_compute_lut

            std::shared_ptr<ShaderModule>
                compute_shader {
                    new ShaderModule("D:/MoCheng/MoChengRT/shaders/PBR/IBL/lookup_table.comp.spv")
                };

            contexts[Compute]->prepare_pipeline({ compute_shader },
                                                { Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute) },
                                                sizeof(PushContant_Compute));

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
std::shared_ptr<CommandBuffer> raster_context_pbr::Begin_Frame()
{
    CommandManager::ExecuteCmd(Context::Get_Singleton()
                                   ->get_device()
                                   ->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   cmd.updateBuffer<Camera_matrix>(camera_matrix->buffer->get_handle(),
                                                                   0,
                                                                   Camera_matrix {
                                                                       .view { m_camera->Get_v_matrix() },
                                                                       .project { m_camera->Get_p_matrix() } });
                               });
    return raster_context::Begin_Frame();
}
void raster_context_pbr::EndFrame()
{
    raster_context::EndFrame();
}
std::shared_ptr<CommandBuffer> raster_context_pbr::BeginSkyboxFrame()
{
    raster_context::EndFrame();
    return nullptr;
}
void raster_context_pbr::EndSkyboxFrame()
{
    raster_context::EndFrame();
}

std::shared_ptr<CommandBuffer> raster_context_pbr::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto render_context = std::reinterpret_pointer_cast<RenderContext>(contexts[Graphic]);

    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        // { // sky_box
        //     cmd->get_handle()
        //         .bindPipeline(vk::PipelineBindPoint::eGraphics,
        //                       render_context->get_pipeline2()->get_handle());
        //     render_context->record_command(cmd);
        //     cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
        //                                          render_context->get_pipeline()->get_layout(),
        //                                          0,
        //                                          { Descriptor_Manager::Get_Singleton()
        //                                                ->get_DescriptorSet(Descriptor_Manager::Graphic)
        //                                                ->get_handle() },
        //                                          {});

        //     cmd->get_handle().bindIndexBuffer(skybox_mesh->get_indices_buffer()->get_handle(),
        //                                       0,
        //                                       vk::IndexType ::eUint32);
        //     cmd->get_handle().bindVertexBuffers(0, {
        //                                                skybox_mesh->get_vertex_buffer()->get_handle(),
        //                                            },
        //                                         { 0 });
        //     auto res = Context::Get_Singleton()
        //                    ->get_camera()
        //                    ->Get_v_matrix();
        //     res[3] = { 0, 0, 0, 1 };
        //     pc = PC_Raster {
        //         // .model_matrix {
        //         //     glm::rotate(skybox_mesh->get_model_matrix(), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f))
        //         // },

        //         .model_matrix { res },

        //         .view_matrix { Context::Get_Singleton()
        //                            ->get_camera()
        //                            ->Get_v_matrix() },
        //         .texture_index = skybox_mesh->m_material.color_texture_index,
        //         .light_pos { 0, 0, 5 },
        //     };
        //     // angle++;

        //     cmd->get_handle()
        //         .pushConstants<PC_Raster>(
        //             render_context
        //                 ->get_pipeline()
        //                 ->get_layout(),
        //             vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        //             0,
        //             pc);
        //     cmd->get_handle()
        //         .drawIndexed(skybox_mesh->get_vertex_count(),
        //                      1,
        //                      0,
        //                      0,
        //                      0);
        // }
        auto res = Context::Get_Singleton()
                       ->get_camera()
                       ->Get_v_matrix();
        res[3] = { 0, 0, 0, 1 };
        SkyboxPass(cmd, [&](std::shared_ptr<CommandBuffer> cmd) {
            render_context->record_command(cmd);
        });

        {
            cmd->get_handle()
                .bindPipeline(vk::PipelineBindPoint ::eGraphics,
                              render_context->get_pipeline()->get_handle());
            render_context->record_command(cmd);

            for (auto& mesh : Mesh::meshs) {
                cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                     render_context->get_pipeline()->get_layout(),
                                                     0,
                                                     { Descriptor_Manager::Get_Singleton()
                                                           ->get_DescriptorSet(Descriptor_Manager::Graphic)
                                                           ->get_handle() },
                                                     {});

                cmd->get_handle().bindIndexBuffer(mesh->get_indices_buffer()->get_handle(),
                                                  0,
                                                  vk::IndexType ::eUint32);
                cmd->get_handle().bindVertexBuffers(0, {
                                                           mesh->get_vertex_buffer()->get_handle(),
                                                       },
                                                    { 0 });
                // std::cout << mesh->get_vertex_buffer()->get_handle() << std::endl;
                auto m = glm::mat4(1);
                auto pos = mesh->get_pos();
                m[3] = glm::vec4(pos, 1);
                pc = PC_Raster {
                    .model_matrix { m },

                    .view_matrix { Context::Get_Singleton()
                                       ->get_camera()
                                       ->Get_v_matrix() },

                    .camera_pos = glm::vec4(m_camera->get_pos(), 1),
                    .color_texture_index = mesh->m_material.color_texture_index,
                    .metallicness_roughness_texture_index = mesh->m_material.metallicness_roughness_texture_index,
                    .normal_texture_index = mesh->m_material.normal_texture_index,
                    .light_pos = { light_pos_x, light_pos_y, light_pos_z, 1 },
                    .use_normal_map = use_normal_map,
                    .use_r_m_map = use_r_rm_map,
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
    }

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