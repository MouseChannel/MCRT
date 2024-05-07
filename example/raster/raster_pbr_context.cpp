#include "example/raster/raster_pbr_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/ComputePass.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RaytracingPass.hpp"
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

#include "Rendering/AntiAliasing/TAA/TAA_Manager.hpp"
#include "Rendering/PBR/IBL_Manager.hpp"
#include "Wrapper/SubPass/OpacitySubPass.hpp"
#include "Wrapper/SubPass/SkyboxSubPass.hpp"
#include <Helper/Model_Loader/ImageWriter.hpp>
#include <execution>

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::raster_context_pbr };
float raster_context_pbr::light_pos_x = 0, raster_context_pbr::light_pos_y = 0, raster_context_pbr::light_pos_z = 5, raster_context_pbr::gamma = 2.2f;
bool raster_context_pbr::use_normal_map = false, raster_context_pbr::use_r_rm_map = false, raster_context_pbr::use_ao = false;
int irradiance_size = 512;

raster_context_pbr::raster_context_pbr()
{

    std::vector<int> a { 1, 2, 3, 4, 5, 6 };
    // std::for_each(std::execution::par,a.begin(),
    //               a.end(),
    //               [](auto& it) {
    //                   std::cout << it << std::endl;
    //               });
    int dd = 0;
}

raster_context_pbr::~raster_context_pbr()
{
}

void raster_context_pbr::prepare(std::shared_ptr<Window> window)
{
    raster_context::prepare(window);
    m_camera->m_position.z = 3;
    sky_box.reset(new Skybox("assets/Cubemap/rainforest_trail_4k.hdr"));
    skybox_mesh = GLTF_Loader::load_skybox("assets/cube.gltf");

    GLTF_Loader::load_model("assets/pbr/cat.glb");

    // GLTF_Loader::load_model("C:\\Users\\moche\\Documents\\sponza.glb");

    //    GLTF_Loader::load_model("C:\\Users\\moche\\Downloads\\pipe_wrench_1k.gltf\\pipe_wrench_1k.gltf");
    // GLTF_Loader::load_model("assets/pbr/temp/untitl1.gltf");
    // Obj_loader::load_model("assets/untitled.obj");
    IBLManager::Get_Singleton()->Init(sky_box);
    // NoiseManager::Get_Singleton()->InitPerlinNoise();

    PASS.resize(1);

    {

        PASS[Pass_index::Graphic] = std::shared_ptr<GraphicContext> { new GraphicContext(m_device) };
        Context::Get_Singleton()
            ->get_graphic_context()
            ->set_constants_size(sizeof(PC_Raster));
        auto graphic_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
        if (graphic_context == nullptr) {
            throw std::runtime_error("not graphic context");
        }

        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::Main_VERT].reset(
            new ShaderModule("example/raster/shader/raster.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Main_FRAG].reset(
            new ShaderModule("example/raster/shader/raster.frag.spv"));
        graphic_shader_modules[Graphic_Pipeline::Skybox_VERT].reset(
            new ShaderModule("example/raster/shader/skybox.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Skybox_FRAG].reset(
            new ShaderModule("example/raster/shader/skybox.frag.spv"));
        graphic_context->prepare();
        graphic_context->prepare_descriptorset([&]() {
            auto descriptor_manager = graphic_context->get_descriptor_manager();
            
            descriptor_manager->Make_DescriptorSet(
                camera_matrix,
                (int)Graphic_Binding::e_camera_matrix,
                DescriptorManager::Graphic);

            descriptor_manager
                ->Make_DescriptorSet(
                    sky_box->get_handle(),
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_skybox,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);

            descriptor_manager
                ->Make_DescriptorSet(
                    Texture::get_image_handles(),
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_textures,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);

            descriptor_manager
                ->Make_DescriptorSet(
                    std::vector { IBLManager::Get_Singleton()->get_LUT() },
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_LUT_image,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);

            descriptor_manager
                ->Make_DescriptorSet(
                    std::vector { IBLManager::Get_Singleton()->get_irradiance()->get_handle() },
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_irradiance_image,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
        });
 


        {
            auto swapchain_renderTarget = graphic_context->AddSwapchainRenderTarget();
            auto depth_renderTarget = graphic_context->AddDepthRenderTarget();
            //            int skyboxSubpass_index = -1;
            //            int opacitySubpass_index = -1;
            //            graphic_context->AddSubPass()
            {
                std::shared_ptr<SkyboxSubPass> skyboxSubpass { new SkyboxSubPass(graphic_context) };
                graphic_context->AddSubPass(skyboxSubpass);

                skyboxSubpass->prepare_vert_shader_module("example/raster/shader/skybox.vert.spv");
                skyboxSubpass->prepare_frag_shader_module("example/raster/shader/skybox.frag.spv");
                skyboxSubpass->prepare_pipeline(sizeof(PC_Raster));
                skyboxSubpass->link_renderTarget({ swapchain_renderTarget },
                                                 {},
                                                 {});
                //            skyboxSubpass->set_subpassDescription(vk::SubpassDescription().setColorAttachments());
//                skyboxSubpass->set_subpassDependency(vk::SubpassDependency()
//                                                         .setSrcSubpass(VK_SUBPASS_EXTERNAL)
//                                                         .setDstSubpass(skyboxSubpass->index)
//                                                         .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
//                                                         .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput)
//                                                         .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite));
                graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                                                          .setDstSubpass(skyboxSubpass->subpass_index)
                                                          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                          .setSrcAccessMask(vk::AccessFlagBits::eNone)
                                                          .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                          .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite));

                // opacity pass
                std::shared_ptr<OpacitySubPass> opacitySubPass { new OpacitySubPass(graphic_context) };
                graphic_context->AddSubPass(opacitySubPass);
                opacitySubPass->prepare_vert_shader_module("example/raster/shader/raster.vert.spv");
                opacitySubPass->prepare_frag_shader_module("example/raster/shader/raster.frag.spv");
                opacitySubPass->prepare_pipeline(sizeof(PC_Raster));
                opacitySubPass->link_renderTarget({ swapchain_renderTarget },
                                                  { depth_renderTarget },
                                                  {});
           
                graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                          .setSrcSubpass(skyboxSubpass->subpass_index)
                                                          .setDstSubpass(opacitySubPass->subpass_index)
                                                          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput )
                                                          .setSrcAccessMask(vk::AccessFlagBits::eNone)
                                                          .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                          .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite));
                
            }
        }

              graphic_context->post_prepare();
    }

    {
        IBLManager::Get_Singleton()->pre_compute_irradiance();
        IBLManager::Get_Singleton()->pre_compute_LUT();
        // ImageWriter::WriteImage(IBLManager::Get_Singleton()->get_LUT());
        // ImageWriter::WriteCubemap(IBLManager::Get_Singleton()->get_irradiance()->get_handle());
    }
    {
        // test
        //        TAA_Manager::Get_Singleton()->Init(get_graphic_context()
        //                                               ->get_present_render_target()
        //                                               //                                               ->get_gbuffer_target()
        //                                               ->Get_Image());
    }
}

std::shared_ptr<CommandBuffer> raster_context_pbr::Begin_Frame()
{
    //    TAA_Manager::Get_Singleton()->TAA_Pass(get_graphic_context()->get_present_render_target()->Get_Image(),
    //                                           get_graphic_context()->get_depth_render_target()->Get_Image(),
    //                                           get_graphic_context()->get_gbuffer_target()->Get_Image());

    //    CommandManager::ExecuteCmd(Context::Get_Singleton()
    //                                   ->get_device()
    //                                   ->Get_Graphic_queue(),
    //                               [&](vk::CommandBuffer& cmd) {
    //                                   cmd.updateBuffer<Camera_matrix>(camera_matrix->buffer->get_handle(),
    //                                                                   0,
    //                                                                   Camera_matrix {
    //                                                                       .view { m_camera->Get_v_matrix() },
    //                                                                       .project { m_camera->Get_p_matrix() },
    //                                                                       .camera_pos { m_camera->get_pos() } });
    //                               });

    {
        //        cmd->get_handle().updateBuffer<Camera_matrix>(camera_matrix->buffer->get_handle(),
        //                                                      0,
        //                                                      Camera_matrix {
        //                                                          .view { m_camera->Get_v_matrix() },
        //                                                          .project { m_camera->Get_p_matrix() },
        //                                                          .camera_pos { m_camera->get_pos() } });
    }

    return raster_context::Begin_Frame();
}

void raster_context_pbr::EndFrame()
{
    raster_context::EndFrame();
}

std::shared_ptr<CommandBuffer> raster_context_pbr::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto render_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);

    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        cmd->get_handle().updateBuffer<Camera_matrix>(camera_matrix->buffer->get_handle(),
                                                      0,
                                                      Camera_matrix {
                                                          .view { m_camera->Get_v_matrix() },
                                                          .project { m_camera->Get_p_matrix() },
                                                          .camera_pos { m_camera->get_pos() } });
    }
    render_context->Begin_RenderPass(cmd);
    {

        {
            {
                auto& skyboxSubpass = render_context->get_subpasses()[0];
                cmd->get_handle().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                               skyboxSubpass->get_pipeline()->get_handle());
                render_context->record_command(cmd);
                //            for (auto& mesh : Mesh::meshs) {
                cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                     skyboxSubpass->get_pipeline()->get_layout(),
                                                     0,
                                                     { get_graphic_context()
                                                           ->get_descriptor_manager()
                                                           ->get_DescriptorSet(
                                                               DescriptorManager::Graphic)
                                                           ->get_handle() },
                                                     {});

                cmd->get_handle().bindIndexBuffer(get_skybox_mesh()
                                                      ->get_indices_buffer()
                                                      ->get_handle(),
                                                  0,
                                                  vk::IndexType::eUint32);

                cmd->get_handle().bindVertexBuffers(0,
                                                    {
                                                        get_skybox_mesh()->get_vertex_buffer()->get_handle(),
                                                    },
                                                    { 0 });
                cmd->get_handle()
                    .pushConstants<PC_Raster>(
                        skyboxSubpass->get_pipeline()->get_layout(),
                        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                        0,
                        PC_Raster {
                            //                .view_matrix { Context::Get_Singleton()
                            //                                   ->get_camera()
                            //                                   ->Get_v_matrix() },
                            .color_texture_index = get_skybox_mesh()->m_material.color_texture_index,
                            .light_pos { 0, 0, 5, 1 },
                        });
                auto rr = get_skybox_mesh()->m_material.color_texture_index;
                //            render_context->record_command(cmd);
                cmd->get_handle()
                    .drawIndexed(get_skybox_mesh()->get_vertex_count(),
                                 1,
                                 0,
                                 0,
                                 0);
            }

            {
                cmd->get_handle().nextSubpass(vk::SubpassContents::eInline);
                for (auto& mesh : Mesh::meshs) {
                    auto& opacitySubpass = render_context->get_subpasses()[1];
                    cmd->get_handle().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                   opacitySubpass->get_pipeline()->get_handle());
                    render_context->record_command(cmd);
                    //            for (auto& mesh : Mesh::meshs) {
                    cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                         opacitySubpass->get_pipeline()->get_layout(),
                                                         0,
                                                         { get_graphic_context()
                                                               ->get_descriptor_manager()
                                                               ->get_DescriptorSet(
                                                                   DescriptorManager::Graphic)
                                                               ->get_handle() },
                                                         {});

                    cmd->get_handle().bindIndexBuffer(mesh
                                                          ->get_indices_buffer()
                                                          ->get_handle(),
                                                      0,
                                                      vk::IndexType::eUint32);

                    cmd->get_handle().bindVertexBuffers(0,
                                                        {
                                                            mesh->get_vertex_buffer()->get_handle(),
                                                        },
                                                        { 0 });
                    auto m = glm::mat4(1);
//                    m[3][3] = 1;
//                    auto pos = mesh->get_pos();
//                    m[3] = glm::vec4(pos, 1);

                    // use_r_rm_map = !use_r_rm_map;
                    pc = PC_Raster {
                        .model_matrix { m },
                        //
                        //                    .view_matrix { Context::Get_Singleton()
                        //                                       ->get_camera()
                        //                                       ->Get_v_matrix() },
                        //
                        //                    .camera_pos = glm::vec4(m_camera->get_pos(), 1),
                        .color_texture_index = mesh->m_material.color_texture_index,
                        .metallicness_roughness_texture_index = mesh->m_material.metallicness_roughness_texture_index,
                        .normal_texture_index = mesh->m_material.normal_texture_index,
                        .light_pos = { light_pos_x, light_pos_y, light_pos_z, 1 },
                        .use_normal_map = use_normal_map,
                        .use_r_m_map = use_r_rm_map,
                        .use_AO = use_ao,
                        .gamma = gamma
                    };
                    cmd->get_handle()
                        .pushConstants<PC_Raster>(
                            opacitySubpass->get_pipeline()->get_layout(),
                            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                            0,
                            pc);
                    //            render_context->record_command(cmd);
                    cmd->get_handle()
                        .drawIndexed(mesh->get_vertex_count(),
                                     1,
                                     0,
                                     0,
                                     0);
                }
            }


            return cmd;
        }
    }
}
void raster_context_pbr::EndGraphicFrame()
{
    auto& m_render_context = PASS[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}
}