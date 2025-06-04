#include "Imgui/imgui.h"
#include "Wrapper/RenderPass.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
#include "raster_pbr_context.hpp"

#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/ComputeContext.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Texture.hpp"
#include <memory>

#include "Shader/PBR/IBL/binding.h"

#include "Shader/PBR/IBL/push_constants.h"
#include "shader/Binding.h"

#include "shader/Constants.h"

#include "Helper/DescriptorSetTarget/ImageDescriptorTarget.hpp"
#include "Rendering/AntiAliasing/TAA/TAA_Manager.hpp"
#include "Rendering/PBR/IBL_Manager.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/GraphicPass/UiPass.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include <Helper/Model_Loader/ImageWriter.hpp>
// #include <vulkan/vulkan.hpp>
namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::raster_context_pbr };
float raster_context_pbr::light_pos_x = 0, raster_context_pbr::light_pos_y = 0, raster_context_pbr::light_pos_z = 5, raster_context_pbr::gamma = 2.2f;
bool raster_context_pbr::use_normal_map = false, raster_context_pbr::use_r_rm_map = false, raster_context_pbr::use_ao = false;
int irradiance_size = 512;

enum DescriptorSetsIndex {
    MAIN,
    INPUT,
    DESCRIPTORSET_COUNT
};

enum PASS {
    eGbufferPass,
    eOpacityPass,
    eToneMapPass,
    eUIPass,
    PassCount
};

enum PIPELINE {
    eSkyboxPipeline,

    eGbufferPipeline,
    eOpacityPipeline,
    eToneMapPipeline,
    eUIPipeline,
    PipelineCount
};

struct OpacityBlendAttachmentState : vk::PipelineColorBlendAttachmentState {

    OpacityBlendAttachmentState()
    {
        setBlendEnable(false)
            .setColorWriteMask(
                vk::ColorComponentFlagBits::eA |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcColorBlendFactor(vk::BlendFactor::eZero)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eZero)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);
    }
};

struct GBufferDepthStencilState : vk::PipelineDepthStencilStateCreateInfo {
    GBufferDepthStencilState()
    {
        setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eLessOrEqual);
        setStencilTestEnable(true)
            .setBack(vk::StencilOpState()
                         .setReference(1)
                         .setCompareOp(vk::CompareOp::eAlways)
                         .setPassOp(vk::StencilOp::eReplace)
                         .setDepthFailOp(vk::StencilOp::eReplace)
                         .setFailOp(vk::StencilOp::eReplace)
                         .setWriteMask(0xff)
                         .setCompareMask(0xff))
            .setFront(back);
    }
};
struct PBRComposeDepthStencilState : vk::PipelineDepthStencilStateCreateInfo {
    PBRComposeDepthStencilState()
    {
        setDepthTestEnable(false)
            .setDepthWriteEnable(false);
        setStencilTestEnable(true)
            .setBack(vk::StencilOpState()
                         .setReference(1)
                         .setCompareOp(vk::CompareOp::eEqual)
                         .setPassOp(vk::StencilOp::eKeep)
                         .setDepthFailOp(vk::StencilOp::eKeep)
                         .setFailOp(vk::StencilOp::eKeep)
                         .setWriteMask(0xff)
                         .setCompareMask(0xff))
            .setFront(back);
        ;
    }
};

raster_context_pbr::raster_context_pbr()
{
}

raster_context_pbr::~raster_context_pbr()
{
}

void raster_context_pbr::prepare(std::shared_ptr<Window> window)
{
    raster_context::prepare(window);

    auto skybox_mesh = GLTF_Loader::load_skybox("../assets/skybox.gltf");

    GLTF_Loader::load_model("../assets/pbr/korean_fire_extinguisher_01_4k.glb");

    IBLManager::Get_Singleton()->Init("../assets/Cubemap/rainforest_trail_4k.hdr");

    PASS.resize(1);

    {
        PASS[Pass_index::Graphic] = std::shared_ptr<GraphicContext> { new GraphicContext(m_device) };

        auto graphic_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
        if (graphic_context == nullptr) {
            throw std::runtime_error("not graphic context");
        }

        graphic_context->prepare();

        {
            // graphic_context->descriptorSets.resize(2);

            auto swapchain_renderTarget = graphic_context->AddSwapchainRenderTarget();
            auto color_renderTarget = graphic_context->AddColorRenderTarget();
            auto resolve_renderTarget = graphic_context->AddResolveRenderTarget();
            auto gbuffer_pos_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Unorm);
            auto gbuffer_normal_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Snorm);

            auto gbuffer_albedo_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Unorm);
            auto gbuffer_arm_renderTarget = graphic_context->AddGbufferRenderTarget(vk::Format::eR8G8B8A8Unorm);
            auto depth_renderTarget = graphic_context->AddDepthRenderTarget();

            {
                graphic_context->descriptorSets.resize(DESCRIPTORSET_COUNT);
                graphic_context->descriptorSetPools.resize(DESCRIPTORSET_COUNT);
                graphic_context->descriptorSets[MAIN] = std::make_shared<DescriptorSet>();
                graphic_context->descriptorSets[INPUT] = std::make_shared<DescriptorSet>();
            }
            {
                graphic_context->graphicPass.resize(PassCount);

                for (int i = 0; i < PassCount; i++) {

                    graphic_context->graphicPass[i] = std::make_shared<GraphicPass>(graphic_context.get());
                    graphic_context->graphicPass[i]->set_subpass_index(i);
                }
            }

            {
                // mocheng

                {

                    graphic_context->descriptorSets[MAIN]->AddBufferDescriptorTarget(camera_matrix->buffer,
                                                                                     e_camera_matrix,
                                                                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                                                                                     vk::DescriptorType::eUniformBuffer);
                    graphic_context->descriptorSets[MAIN]->AddImageDescriptorTarget(IBLManager::Get_Singleton()->get_skybox(),
                                                                                    e_skybox,
                                                                                    vk::ShaderStageFlagBits::eFragment,
                                                                                    vk::DescriptorType::eCombinedImageSampler);
                    graphic_context->descriptorSets[MAIN]->AddImageDescriptorTarget(Texture::get_image_handles(),
                                                                                    e_textures,
                                                                                    vk::ShaderStageFlagBits::eFragment,
                                                                                    vk::DescriptorType::eCombinedImageSampler);

                    graphic_context->descriptorSets[MAIN]->AddImageDescriptorTarget(IBLManager::Get_Singleton()->get_LUT(),
                                                                                    e_LUT_image,
                                                                                    vk::ShaderStageFlagBits::eFragment,
                                                                                    vk::DescriptorType::eCombinedImageSampler);
                    graphic_context->descriptorSets[MAIN]->AddImageDescriptorTarget(IBLManager::Get_Singleton()->get_irradiance(),
                                                                                    e_irradiance_image,
                                                                                    vk::ShaderStageFlagBits::eFragment,
                                                                                    vk::DescriptorType::eCombinedImageSampler);
                    graphic_context->descriptorSets[INPUT]->AddInputImageDescriptorTarget(graphic_context->all_rendertargets,
                                                                                          color_renderTarget,
                                                                                          e_gbuffer_color_renderTarget);
                    graphic_context->descriptorSets[INPUT]->AddInputImageDescriptorTarget(graphic_context->all_rendertargets,
                                                                                          gbuffer_pos_renderTarget,
                                                                                          e_gbuffer_pos_renderTarget);
                    graphic_context->descriptorSets[INPUT]->AddInputImageDescriptorTarget(graphic_context->all_rendertargets,
                                                                                          gbuffer_albedo_renderTarget,
                                                                                          e_gbuffer_albedo_renderTarget);
                    graphic_context->descriptorSets[INPUT]->AddInputImageDescriptorTarget(graphic_context->all_rendertargets,
                                                                                          gbuffer_normal_renderTarget,
                                                                                          e_gbuffer_normal_renderTarget);
                    graphic_context->descriptorSets[INPUT]->AddInputImageDescriptorTarget(graphic_context->all_rendertargets,
                                                                                          gbuffer_arm_renderTarget,
                                                                                          e_gbuffer_arm_renderTarget);

                    if (auto inputSet = graphic_context->descriptorSets[INPUT]; inputSet->check_dirty()) {
                        graphic_context->descriptorSetPools[INPUT].reset(new DescriptorPool({ inputSet }));
                        inputSet->build(graphic_context->descriptorSetPools[INPUT], graphic_context->get_frame_count());
                    }

                    if (graphic_context->descriptorSets[MAIN]->check_dirty()) {
                        graphic_context->descriptorSetPools[MAIN].reset(new DescriptorPool({ graphic_context->descriptorSets[MAIN] }, graphic_context->get_frame_count()));
                        graphic_context->descriptorSets[MAIN]->build(graphic_context->descriptorSetPools[MAIN], graphic_context->get_frame_count());
                        // graphic_context->descriptorSets[MAIN]->build(graphic_context->descriptorSetPools[MAIN], 1);
                    }

                    auto gbufferPass = graphic_context->graphicPass[eGbufferPass];
                    gbufferPass->link_renderTarget({ color_renderTarget,
                                                     gbuffer_albedo_renderTarget,
                                                     gbuffer_pos_renderTarget,
                                                     gbuffer_normal_renderTarget,
                                                     gbuffer_arm_renderTarget },
                                                   { depth_renderTarget },
                                                   {},
                                                   {});
                }
                {
                    auto opacityPass = graphic_context->graphicPass[eOpacityPass];
                    opacityPass->link_renderTarget({ color_renderTarget },
                                                   { depth_renderTarget },
                                                   {

                                                       gbuffer_albedo_renderTarget,
                                                       gbuffer_pos_renderTarget,
                                                       gbuffer_normal_renderTarget,
                                                       gbuffer_arm_renderTarget,

                                                   },
                                                   {});
                }
                {

                    auto toneMapPass = graphic_context->graphicPass[eToneMapPass];
                    toneMapPass->link_renderTarget({ swapchain_renderTarget }, {}, { color_renderTarget }, {});
                }
                {
                    auto uiPass = graphic_context->graphicPass[eUIPass];
                    uiPass->link_renderTarget({ swapchain_renderTarget }, {}, {}, {});
                }
                {
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                                                              .setDstSubpass(eGbufferPass)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite));
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                                                              .setDstSubpass(eGbufferPass)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eNone)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests)
                                                              .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite));
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(eGbufferPass)
                                                              .setDstSubpass(eOpacityPass)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)

                                                              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                                                              .setDstAccessMask(vk::AccessFlagBits::eInputAttachmentRead)
                                                              .setDependencyFlags(vk::DependencyFlagBits::eByRegion));
                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(eOpacityPass)
                                                              .setDstSubpass(eToneMapPass)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
                                                              .setDstAccessMask(vk::AccessFlagBits::eInputAttachmentRead));

                    graphic_context->AddSubPassDependency(vk::SubpassDependency()
                                                              .setSrcSubpass(eUIPass - 1)
                                                              .setDstSubpass(eUIPass)
                                                              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                                                              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                                                              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite));
                }
            }
        }

        auto gbufferPass = graphic_context->graphicPass[eGbufferPass];

        graphic_context->post_prepare();

        auto uiPass = std::reinterpret_pointer_cast<UIPass>(graphic_context->graphicPass[eUIPass]);
        uiPass->Init();
        {
            // pipelines

            auto renderPass = graphic_context->Get_render_pass();
            graphic_context->m_pipelines.resize(PipelineCount);
            auto gbufferPass = graphic_context->graphicPass[eGbufferPass];
            auto opacityPass = graphic_context->graphicPass[eOpacityPass];
            auto toneMapPass = graphic_context->graphicPass[eToneMapPass];
            auto& descriptorSets = graphic_context->descriptorSets;
            graphic_context->m_pipelines[eSkyboxPipeline].reset(new Graphic_Pipeline(renderPass,
                                                                                     "../example/raster/shader/skybox.vert.spv",
                                                                                     "../example/raster/shader/skybox.frag.spv",
                                                                                     vk::CullModeFlagBits::eNone,
                                                                                     vk::PipelineDepthStencilStateCreateInfo()
                                                                                         .setDepthTestEnable(false)
                                                                                         .setDepthWriteEnable(false)
                                                                                         .setStencilTestEnable(false),
                                                                                     vk::SampleCountFlagBits::e1,
                                                                                     gbufferPass->get_subpass_index(),
                                                                                     { descriptorSets[MAIN] },
                                                                                     sizeof(PC_Raster),
                                                                                     vk::ShaderStageFlagBits::eFragment,
                                                                                     gbufferPass->color_references.size(),
                                                                                     vk::PipelineColorBlendAttachmentState()
                                                                                         .setBlendEnable(false)
                                                                                         .setColorWriteMask(
                                                                                             vk::ColorComponentFlagBits::eR |
                                                                                             vk::ColorComponentFlagBits::eG |
                                                                                             vk::ColorComponentFlagBits::eB |
                                                                                             vk::ColorComponentFlagBits::eA)));
            // OpacityBlendAttachmentState()));

            graphic_context->m_pipelines[eGbufferPipeline]
                .reset(new Graphic_Pipeline(renderPass,
                                            "../example/raster/shader/gbuffer.vert.spv",
                                            "../example/raster/shader/gbuffer.frag.spv",
                                            vk::CullModeFlagBits::eBack,
                                            GBufferDepthStencilState(),
                                            vk::SampleCountFlagBits::e1,
                                            gbufferPass->get_subpass_index(),
                                            { descriptorSets[MAIN], descriptorSets[INPUT] },
                                            sizeof(PC_Raster),
                                            vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
                                            gbufferPass->color_references.size(),
                                            OpacityBlendAttachmentState()));
            graphic_context->m_pipelines[eOpacityPipeline].reset(new Graphic_Pipeline(renderPass,
                                                                                      "../example/raster/shader/tonemap.vert.spv",
                                                                                      "../example/raster/shader/pbrfs.frag.spv",
                                                                                      vk::CullModeFlagBits::eNone,
                                                                                      PBRComposeDepthStencilState(),
                                                                                      vk::SampleCountFlagBits::e1,
                                                                                      opacityPass->get_subpass_index(),
                                                                                      { descriptorSets[MAIN], descriptorSets[INPUT] },
                                                                                      sizeof(PC_Raster),
                                                                                      vk::ShaderStageFlagBits::eFragment,
                                                                                      opacityPass->color_references.size(),
                                                                                      OpacityBlendAttachmentState()));
            graphic_context->m_pipelines[eToneMapPipeline].reset(new Graphic_Pipeline(renderPass,
                                                                                      "../example/raster/shader/tonemap.vert.spv",
                                                                                      "../example/raster/shader/tonemap.frag.spv",
                                                                                      vk::CullModeFlagBits::eNone,
                                                                                      false,
                                                                                      false,
                                                                                      vk::SampleCountFlagBits::e1,
                                                                                      toneMapPass->get_subpass_index(),
                                                                                      { descriptorSets[INPUT] },
                                                                                      sizeof(PC_Raster),
                                                                                      vk::ShaderStageFlagBits::eFragment,
                                                                                      toneMapPass->color_references.size(),
                                                                                      OpacityBlendAttachmentState()));
        }
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

    auto render_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
    std::shared_ptr<CommandBuffer> command = render_context->BeginFrame();
    {

        command->get_handle().updateBuffer<Camera_matrix>(camera_matrix->buffer->get_handle(),
                                                          0,
                                                          Camera_matrix {
                                                              .view { m_camera->Get_v_matrix() },
                                                              .project { m_camera->Get_p_matrix() },

                                                              .camera_pos {
                                                                  m_camera->get_pos() } });
    }

    render_context->Begin_RenderPass(command);
    {

        auto graphic_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
        auto cur_index = graphic_context->get_cur_index();
        auto cur_index_in_swapchain = graphic_context->get_cur_index_in_swapchain();
        auto& pipelines = graphic_context->m_pipelines;
        auto cmd = command->get_handle();
        {
            cmd.setViewport(0,
                            vk::Viewport()
                                .setHeight(extent2d.height)
                                .setWidth(extent2d.width)
                                .setMinDepth(0)
                                .setMaxDepth(1)
                                .setX(0)
                                .setY(0));
            cmd.setScissor(0,
                           vk::Rect2D()
                               .setExtent(extent2d)
                               .setOffset(vk::Offset2D()
                                              .setX(0)
                                              .setY(0)));
            {

                cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines[eSkyboxPipeline]->get_handle());

                cmd.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    pipelines[eSkyboxPipeline]->get_layout(),
                    0,
                    { graphic_context->descriptorSets[MAIN]->get_handle()[graphic_context->get_cur_index_in_swapchain()] },
                    // { graphic_context->descriptorSets[MAIN]->get_handle()[0] },

                    {});

                cmd.bindVertexBuffers(0, IBLManager::Get_Singleton()->get_skybox_mesh()->get_vertex_buffer()->get_handle(), { 0 });
                cmd.bindIndexBuffer(IBLManager::Get_Singleton()->get_skybox_mesh()->get_indices_buffer()->get_handle(), 0, vk::IndexType::eUint32);
                cmd.drawIndexed(IBLManager::Get_Singleton()->get_skybox_mesh()->get_vertex_count(), 1, 0, 0, 0);
            }
        }

        {

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines[eGbufferPipeline]->get_handle());
            cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                pipelines[eGbufferPipeline]->get_layout(),
                0,
                { graphic_context->descriptorSets[MAIN]->get_handle()[graphic_context->get_cur_index_in_swapchain()],
                  // graphic_context->descriptorSets[MAIN]->get_handle()[0],

                  graphic_context->descriptorSets[INPUT]->get_handle()[graphic_context->get_cur_index_in_swapchain()] },
                {});

            for (auto mesh : Mesh::all_meshs) {
                cmd.bindVertexBuffers(0, mesh->get_vertex_buffer()->get_handle(), { 0 });
                cmd.bindIndexBuffer(mesh->get_indices_buffer()->get_handle(), 0, vk::IndexType::eUint32);
                cmd.pushConstants<PC_Raster>(pipelines[eGbufferPipeline]->get_layout(),
                                             vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex,
                                             0,
                                             PC_Raster {
                                                 .color_texture_index = mesh->m_material.color_texture_index,
                                                 .metallicness_roughness_texture_index = mesh->m_material.metallicness_roughness_texture_index,
                                                 .normal_texture_index = mesh->m_material.normal_texture_index });
                cmd.drawIndexed(mesh->get_vertex_count(), 1, 0, 0, 0);
            }
        }
        {
            cmd.nextSubpass(vk::SubpassContents::eInline);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines[eOpacityPipeline]->get_handle());
            cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                pipelines[eOpacityPipeline]->get_layout(),
                0,
                {
                    graphic_context->descriptorSets[MAIN]->get_handle()[cur_index_in_swapchain],
                    // graphic_context->descriptorSets[MAIN]->get_handle()[0],

                    graphic_context->descriptorSets[INPUT]->get_handle()[cur_index_in_swapchain],
                },
                {});

            cmd.draw(3, 1, 0, 0);
        }
        {
            cmd.nextSubpass(vk::SubpassContents::eInline);

            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines[eToneMapPipeline]->get_handle());

            cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                pipelines[eToneMapPipeline]->get_layout(),
                0,
                { graphic_context->descriptorSets[INPUT]->get_handle()[graphic_context->get_cur_index_in_swapchain()] },
                {});

            cmd.draw(3, 1, 0, 0);
        }
        {
            cmd.nextSubpass(vk::SubpassContents::eInline);
            auto uiPass = std::reinterpret_pointer_cast<UIPass>(graphic_context->graphicPass[eUIPass]);

            uiPass->DrawUI(cmd,
                           []() {
                               ImGui::Text("move:[W A S D Q E]");
                               ImGui::Text("Hold left Mouse Button To Rotate!!");
                               ImGui::SliderFloat("move-sensitivity", &Context::Get_Singleton()->get_camera()->m_sensitivity, 1e-2f, 1e-1f);

                               ImGui::Checkbox("use_normal_map", &raster_context_pbr::use_normal_map);
                               ImGui::Checkbox("rm", &use_r_rm_map);
                               ImGui::Checkbox("AO", &raster_context_pbr::use_ao);
                               ImGui::Text("fps       : %7.3f", ImGui::GetIO().Framerate);
                           });
        }
        return command;
    }
}

void raster_context_pbr::EndGraphicFrame()
{

    auto& m_render_context = PASS[Graphic];
    m_render_context->Submit();

    m_render_context->EndFrame();
}
}
