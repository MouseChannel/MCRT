// #include "Wrapper/SubPass/ToneMapSubpass.hpp"
#include "lib/imgui/imgui.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
#include "example/raster/raster_pbr_context.hpp"

#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/ComputeContext.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RaytracingPass.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
// #include "Wrapper/Skybox.hpp"
#include "Wrapper/Texture.hpp"

#include "shaders/PBR/IBL/binding.h"

#include "Wrapper/SubPass/IMGUISubPass.hpp"

#include "example/raster/shader/Binding.h"
#include "shaders/PBR/IBL/push_constants.h"

#include "example/raster/shader/Constants.h"

#include "Helper/DescriptorSetTarget/BufferDescriptorSetTarget.hpp"
#include "Helper/DescriptorSetTarget/ImageDescriptorSetTarget.hpp"

#include "Rendering/AntiAliasing/TAA/TAA_Manager.hpp"
#include "Rendering/PBR/IBL_Manager.hpp"
#include "Wrapper/SubPass/OpacitySubPass.hpp"
#include "Wrapper/SubPass/SkyboxSubPass.hpp"
#include "Wrapper/SubPass/ToneMapSubpass.hpp"
#include "Wrapper/SubPass/TransparencySubPass.hpp"
#include <Helper/Model_Loader/ImageWriter.hpp>

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::raster_context_pbr };
float raster_context_pbr::light_pos_x = 0, raster_context_pbr::light_pos_y = 0, raster_context_pbr::light_pos_z = 5, raster_context_pbr::gamma = 2.2f;
bool raster_context_pbr::use_normal_map = false, raster_context_pbr::use_r_rm_map = false, raster_context_pbr::use_ao = false;
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

    auto skybox_mesh = GLTF_Loader::load_skybox("assets/skybox.gltf");

    // skybox_mesh = Mesh::LoadFromFile("assets/skybox.gltf");
    // Mesh::LoadFromFile("/home/mousechannel/project/MCRT/assets/pbr/gun.gltf");

    // Mesh::LoadFromFile("/home/mousechannel/Downloads/korean_fire_extinguisher_01_4k.gltf/korean_fire_extinguisher_01_4k.gltf");

    Mesh::LoadFromFile("/home/mousechannel/project/MCRT/assets/pbr/korean_fire_extinguisher_01_4k/korean_fire_extinguisher_01_4k.gltf");
    IBLManager::Get_Singleton()->Init("assets/Cubemap/hospital_room_2_8k.hdr");

    PASS.resize(1);

    {
        PASS[Pass_index::Graphic] = std::shared_ptr<GraphicContext> { new GraphicContext(m_device) };
        // Context::Get_Singleton()
        //     ->get_graphic_context()
        //     ->set_constants_size(sizeof(PC_Raster));
        auto graphic_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
        if (graphic_context == nullptr) {
            throw std::runtime_error("not graphic context");
        }

        graphic_context->prepare();

        {
            auto swapchain_renderTarget = graphic_context->AddSwapchainRenderTarget();
            auto color_renderTarget = graphic_context->AddColorRenderTarget();
            auto depth_renderTarget = graphic_context->AddDepthRenderTarget();
            auto resolve_renderTarget = graphic_context->AddResolveRenderTarget();
            // auto gbuffer_color_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Unorm);
            // auto gbuffer_depth_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Unorm);
            // auto gbuffer_normal_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Unorm);

            graphic_context->subpasses.resize(SubPass_index::SubPassCount);
            {

                {
                    graphic_context->subpasses[SkyboxSubPassIndex].reset(new SkyboxSubPass(graphic_context));

                    auto& skyboxSubpass = graphic_context->subpasses[SkyboxSubPassIndex];
                    skyboxSubpass->link_renderTarget({ color_renderTarget },
                                                     {},
                                                     {},
                                                     { resolve_renderTarget });
                    skyboxSubpass->prepare_vert_shader_module("example/raster/shader/skybox.vert.spv");
                    skyboxSubpass->prepare_frag_shader_module("example/raster/shader/skybox.frag.spv");
                    skyboxSubpass->Prepare_DescriptorSet(
                        [&]() {
                            skyboxSubpass->AddDescriptorSetTarget(std::make_shared<BufferDescriptorSetTarget>(
                                camera_matrix->buffer,
                                (int)Graphic_Binding::e_camera_matrix,
                                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eUniformBuffer,
                                skyboxSubpass->get_DescriptorSet()));

                            skyboxSubpass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
                                IBLManager::Get_Singleton()->get_skybox(),
                                // Which_Set::Graphic,
                                (int)Graphic_Binding::e_skybox,
                                vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eCombinedImageSampler,
                                skyboxSubpass->get_DescriptorSet()));
                        }

                    );
                    skyboxSubpass->prepare_pipeline(sizeof(PC_Raster));
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                                                              .setDstSubpass(SkyboxSubPassIndex)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                                                              .setDstAccessMask(vk::AccessFlagBits::eShaderRead));
                } // opacity pass

                {
                    graphic_context->subpasses[OpacitySubPassIndex].reset(new OpacitySubPass(graphic_context));
                    // graphic_context->AddSubPass(opacitySubPass);
                    auto& opacitySubPass = graphic_context->subpasses[OpacitySubPassIndex];
                    opacitySubPass->link_renderTarget({ color_renderTarget },
                                                      { depth_renderTarget },
                                                      {},
                                                      { resolve_renderTarget });
                    opacitySubPass->prepare_vert_shader_module("example/raster/shader/pbrfs.vert.spv");
                    opacitySubPass->prepare_frag_shader_module("example/raster/shader/pbrfs.frag.spv");
                    opacitySubPass->Prepare_DescriptorSet(
                        [&]() {
                            opacitySubPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
                                IBLManager::Get_Singleton()->get_skybox(),
                                // Which_Set::Graphic,
                                (int)Graphic_Binding::e_skybox,
                                vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eCombinedImageSampler,
                                opacitySubPass->get_DescriptorSet()));
                            opacitySubPass->AddDescriptorSetTarget(std::make_shared<BufferDescriptorSetTarget>(
                                camera_matrix->buffer,
                                (int)Graphic_Binding::e_camera_matrix,
                                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eUniformBuffer,
                                opacitySubPass->get_DescriptorSet()));
                            opacitySubPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
                                Texture::get_image_handles(),
                                (int)Graphic_Binding::e_textures,
                                vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eCombinedImageSampler,
                                opacitySubPass->get_DescriptorSet()));

                            opacitySubPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
                                IBLManager::Get_Singleton()->get_LUT(),
                                (int)Graphic_Binding::e_LUT_image,
                                vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eCombinedImageSampler,
                                opacitySubPass->get_DescriptorSet()));
                            opacitySubPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
                                IBLManager::Get_Singleton()->get_irradiance(),
                                (int)Graphic_Binding::e_irradiance_image,
                                vk::ShaderStageFlagBits::eFragment,
                                vk::DescriptorType::eCombinedImageSampler,
                                opacitySubPass->get_DescriptorSet()));
                        }

                    );
                    opacitySubPass->prepare_pipeline(sizeof(PC_Raster));

                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(SkyboxSubPassIndex)
                                                              .setDstSubpass(OpacitySubPassIndex)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                                                              .setDstAccessMask(vk::AccessFlagBits::eShaderRead));
                }
                {
                    // graphic_context->subpasses[TransparencySubPassIndex].reset(new TransparencySubPass(graphic_context));

                    // auto& transparencySubPass = graphic_context->subpasses[TransparencySubPassIndex];

                    // transparencySubPass->link_renderTarget({ swapchain_renderTarget },
                    //                                        { depth_renderTarget },
                    //                                        {});
                    // transparencySubPass->prepare_vert_shader_module("example/raster/shader/raster.vert.spv");
                    // transparencySubPass->prepare_frag_shader_module("example/raster/shader/raster.frag.spv");
                    // transparencySubPass->prepare_pipeline(sizeof(PC_Raster));
                    // graphic_context->AddSubPassDependency(vk::SubpassDependency()
                    //                                           .setSrcSubpass(OpacitySubPassIndex)
                    //                                           .setDstSubpass(TransparencySubPassIndex)
                    //                                           .setSrcStageMask(vk::PipelineStageFlagBits::eLateFragmentTests)
                    //                                           .setSrcAccessMask(vk::AccessFlagBits::eNone)
                    //                                           .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput)
                    //                                           .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite));
                }
                {
                    graphic_context->subpasses[ToneMapSubPassIndex].reset(new ToneMapSubPass(graphic_context));
                    auto& tonemapSubPass = graphic_context->subpasses[ToneMapSubPassIndex];
                    tonemapSubPass->link_renderTarget({ swapchain_renderTarget },
                                                      {},
                                                      { resolve_renderTarget },
                                                      {});
                    tonemapSubPass->prepare_vert_shader_module("example/raster/shader/tonemap.vert.spv");
                    tonemapSubPass->prepare_frag_shader_module("example/raster/shader/tonemap.frag.spv");
                    tonemapSubPass->Prepare_DescriptorSet(
                        [=]() {
                            for (int i = 0; i < tonemapSubPass->get_DescriptorSetCount(); i++) {
               
                                auto input_renderTarget = graphic_context->Get_render_targets(i)[resolve_renderTarget];
                                tonemapSubPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
                                    // IBLManager::Get_Singleton()->get_skybox(),
                                    std::vector { input_renderTarget->Get_Image()->Get_Image_View() },
                                    std::vector { input_renderTarget->get_inputLayout() },
                                    // Which_Set::Graphic,
                                    (int)Graphic_Binding::e_tonemap_input,
                                    vk::ShaderStageFlagBits::eFragment,
                                    vk::DescriptorType::eInputAttachment,
                                    tonemapSubPass->get_DescriptorSet(),
                                    i));
                            }
                        }

                    );
                    tonemapSubPass->prepare_pipeline(sizeof(PC_Raster));
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(OpacitySubPassIndex)
                                                              .setDstSubpass(ToneMapSubPassIndex)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                                                              .setDstAccessMask(vk::AccessFlagBits::eShaderRead));
                }
                {
                    graphic_context->subpasses[IMGUISubPassIndex].reset(new IMGUISubPass(graphic_context));
                    auto& imguiSubPass = graphic_context->subpasses[IMGUISubPassIndex];
                    // graphic_context->AddSubPass(imguiSubPass);
                    imguiSubPass->link_renderTarget({ swapchain_renderTarget }, {}, {}, {});
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(IMGUISubPassIndex - 1)
                                                              .setDstSubpass(IMGUISubPassIndex)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                                                              .setDstAccessMask(vk::AccessFlagBits::eShaderRead));
                }
            }
        }

        graphic_context->post_prepare();
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
    auto render_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {

        cmd->get_handle().updateBuffer<Camera_matrix>(camera_matrix->buffer->get_handle(),
                                                      0,
                                                      Camera_matrix {
                                                          .view { m_camera->Get_v_matrix() },
                                                          .project { m_camera->Get_p_matrix() },

                                                          .camera_pos {
                                                              m_camera->get_pos() } });
    }
    render_context->Begin_RenderPass(cmd);
    {

        {
            {

                auto& skyboxSubpass = render_context->get_subpasses()[SkyboxSubPassIndex];
                cmd->get_handle().bindPipeline(vk::PipelineBindPoint::eGraphics, skyboxSubpass->get_pipeline()->get_handle());

                cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                     skyboxSubpass->get_pipeline()->get_layout(),
                                                     0,
                                                     skyboxSubpass->get_DescriptorSet()->get_handle()[0],
                                                     {});

                cmd->get_handle().bindIndexBuffer(IBLManager::Get_Singleton()->get_skybox_mesh()->get_indices_buffer()->get_handle(),
                                                  0,
                                                  vk::IndexType::eUint32);

                cmd->get_handle().bindVertexBuffers(0,
                                                    {
                                                        IBLManager::Get_Singleton()->get_skybox_mesh()->get_vertex_buffer()->get_handle(),
                                                    },
                                                    { 0 });
                cmd->get_handle().pushConstants<PC_Raster>(skyboxSubpass
                                                               ->get_pipeline()
                                                               ->get_layout(),
                                                           vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                                                           0,
                                                           PC_Raster {

                                                           });
                auto extent2d = Context::Get_Singleton()->get_extent2d();

                cmd->get_handle().setViewport(0,
                                              vk::Viewport()
                                                  .setHeight(extent2d.height)
                                                  .setWidth(extent2d.width)
                                                  .setMinDepth(0)
                                                  .setMaxDepth(1)
                                                  .setX(0)
                                                  .setY(0));
                cmd->get_handle().setScissor(0,
                                             vk::Rect2D()
                                                 .setExtent(extent2d)
                                                 .setOffset(vk::Offset2D()
                                                                .setX(0)
                                                                .setY(0)));
                cmd->get_handle().drawIndexed(IBLManager::Get_Singleton()->get_skybox_mesh()->get_vertex_count(),
                                              1,
                                              0,
                                              0,
                                              0);
            }

            {
                cmd->get_handle().nextSubpass(vk::SubpassContents::eInline);
                auto& opacitySubpass = render_context->get_subpasses()[OpacitySubPassIndex];
                auto extent2d = Context::Get_Singleton()->get_extent2d();

                cmd->get_handle().setViewport(0,
                                              vk::Viewport()
                                                  .setHeight(extent2d.height)
                                                  .setWidth(extent2d.width)
                                                  .setMinDepth(0)
                                                  .setMaxDepth(1)
                                                  .setX(0)
                                                  .setY(0));
                cmd->get_handle().setScissor(0,
                                             vk::Rect2D()
                                                 .setExtent(extent2d)
                                                 .setOffset(vk::Offset2D()
                                                                .setX(0)
                                                                .setY(0)));

                auto rr = Mesh::all_meshs;
                for (auto& mesh : Mesh::all_meshs) {
                    cmd->get_handle().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                   opacitySubpass->get_pipeline()->get_handle());

                    cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                         opacitySubpass->get_pipeline()->get_layout(),
                                                         0,
                                                         opacitySubpass->get_DescriptorSet()->get_handle()[0],
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
                    auto m = glm::mat4(1.f);

                    cmd->get_handle()
                        .pushConstants<PC_Raster>(
                            opacitySubpass->get_pipeline()->get_layout(),
                            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                            0,
                            PC_Raster {
                                .model_matrix { m },
                                .light_pos = { light_pos_x, light_pos_y, light_pos_z, 1 },
                                .color_texture_index = mesh->m_material.color_texture_index,
                                .metallicness_roughness_texture_index = mesh->m_material.metallicness_roughness_texture_index,
                                .normal_texture_index = mesh->m_material.normal_texture_index,
                                .use_normal_map = use_normal_map,
                                .use_r_m_map = use_r_rm_map,
                                .use_AO = use_ao,
                                .gamma = gamma });
                    // std::cout<<use_r_rm_map<<std::endl;

                    cmd->get_handle()
                        .drawIndexed(mesh->get_vertex_count(),
                                     1,
                                     0,
                                     0,
                                     0);
                }
            }
            // {
            //     cmd->get_handle().nextSubpass(vk::SubpassContents::eInline);
            //     auto& transparencySubPass = render_context->subpasses[TransparencySubPassIndex];
            //     for (auto& mesh : Mesh::all_meshs) {
            //         cmd->get_handle().bindPipeline(vk::PipelineBindPoint::eGraphics,
            //                                        transparencySubPass->get_pipeline()->get_handle());
            //         render_context->record_command(cmd);
            //         //            for (auto& mesh : Mesh::meshs) {
            //         cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            //                                              transparencySubPass->get_pipeline()->get_layout(),
            //                                              0,
            //                                              { get_graphic_context()
            //                                                    ->get_descriptor_manager()
            //                                                    ->get_DescriptorSet(
            //                                                        DescriptorManager::Graphic)
            //                                                    ->get_handle() },
            //                                              {});
            //         cmd->get_handle().bindIndexBuffer(mesh
            //                                               ->get_indices_buffer()
            //                                               ->get_handle(),
            //                                           0,
            //                                           vk::IndexType::eUint32);
            //         cmd->get_handle().bindVertexBuffers(0,
            //                                             {
            //                                                 mesh->get_vertex_buffer()->get_handle(),
            //                                             },
            //                                             { 0 });
            //         auto m = glm::mat4(1);
            //         //                    m[3][3] = 1;
            //         auto pos = mesh->get_pos();
            //         m[3] = glm::vec4(pos, 1);
            //         // use_r_rm_map = !use_r_rm_map;
            //         pc = PC_Raster {
            //             .model_matrix { m },
            //             .color_texture_index = mesh->m_material.color_texture_index,
            //             .metallicness_roughness_texture_index = mesh->m_material.metallicness_roughness_texture_index,
            //             .normal_texture_index = mesh->m_material.normal_texture_index,
            //             .light_pos = { light_pos_x, light_pos_y, light_pos_z, 1 },
            //             .use_normal_map = use_normal_map,
            //             .use_r_m_map = use_r_rm_map,
            //             .use_AO = use_ao,
            //             .gamma = gamma
            //         };
            //         cmd->get_handle()
            //             .pushConstants<PC_Raster>(
            //                 transparencySubPass->get_pipeline()->get_layout(),
            //                 vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            //                 0,
            //                 PC_Raster {
            //                     .model_matrix { m },
            //                     .color_texture_index = mesh->m_material.color_texture_index,
            //                     .metallicness_roughness_texture_index = mesh->m_material.metallicness_roughness_texture_index,
            //                     .normal_texture_index = mesh->m_material.normal_texture_index,
            //                     .light_pos = { light_pos_x, light_pos_y, light_pos_z, 1 },
            //                     .use_normal_map = use_normal_map,
            //                     .use_r_m_map = use_r_rm_map,
            //                     .use_AO = use_ao,
            //                     .gamma = gamma });
            //         //            render_context->record_command(cmd);
            //         cmd->get_handle()
            //             .drawIndexed(mesh->get_vertex_count(),
            //                          1,
            //                          0,
            //                          0,
            //                          0);
            //     }
            // }
            {
                cmd->get_handle().nextSubpass(vk::SubpassContents::eInline);
                auto tonemapSubpass = std::reinterpret_pointer_cast<ToneMapSubPass>(render_context->get_subpasses()[ToneMapSubPassIndex]);

                cmd->get_handle().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                               tonemapSubpass->get_pipeline()->get_handle());
                cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                     tonemapSubpass->get_pipeline()->get_layout(),
                                                     0,
                                                     tonemapSubpass->get_DescriptorSet()->get_handle()[get_graphic_context()->get_cur_index()],
                                                     //  tonemapSubpass->get_DescriptorSet()->get_handle()[0],

                                                     {});

                cmd->get_handle()
                    .draw(3,
                          1,
                          0,
                          0);
            }
            {
                cmd->get_handle().nextSubpass(vk::SubpassContents::eInline);
                auto imguiSubpass = std::reinterpret_pointer_cast<IMGUISubPass>(render_context->get_subpasses()[IMGUISubPassIndex]);
                imguiSubpass->drawUI(cmd, []() {
                    // ImVec4 color = ImVec4(1, 1, 1, 1);
                    ImGui::Text("move:[W A S D Q E]");
                     ImGui::Text("Hold left Mouse Button To Rotate!!");

                    ImGui::SliderFloat("move-sensitivity", &Context::Get_Singleton()->get_camera()->m_sensitivity, 1e-2f, 1e-1f);
                    // ImGui::SliderFloat("light_pos_y", &raster_context_pbr::light_pos_y, -2.0f, 2.0f);
                    // ImGui::SliderFloat("light_pos_z", &raster_context_pbr::light_pos_z, 0.f, 20.0f);
                    ImGui::Checkbox("use_normal_map", &raster_context_pbr::use_normal_map);
                    ImGui::Checkbox("rm", &use_r_rm_map);
                    ImGui::Checkbox("AO", &raster_context_pbr::use_ao);
                    // ImGui::SliderFloat("gamma", &raster_context_pbr::gamma, 1.0f, 4.4f);
                });
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