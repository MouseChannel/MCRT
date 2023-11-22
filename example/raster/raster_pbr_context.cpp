#include "example/raster/raster_pbr_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/ComputePass.hpp"
#include "Rendering/GraphicPass.hpp"
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
#include <Helper/Model_Loader/ImageWriter.hpp>

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
    sky_box.reset(new Skybox("/home/mocheng/project/MCRT/assets/kart_club_2k.hdr"));
    skybox_mesh = GLTF_Loader::load_skybox("/home/mocheng/project/MCRT/assets/cube.gltf");

    GLTF_Loader::load_model("/home/mocheng/project/MCRT/assets/pbr/cat.gltf");
    // Obj_loader::load_model("/home/mocheng/project/MCRT/assets/untitled.obj");
    IBLManager::Get_Singleton()->Init(sky_box);

    PASS.resize(1);

    {
        PASS[Pass_index::Graphic] = std::shared_ptr<GraphicPass> { new GraphicPass(m_device) };
        Context::Get_Singleton()
            ->get_graphic_context()
            ->set_constants_size(sizeof(PC_Raster));
        auto graphic_context = std::reinterpret_pointer_cast<GraphicPass>(PASS[Graphic]);
        if (graphic_context == nullptr) {
            throw std::runtime_error("not graphic context");
        }
        std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
        graphic_shader_modules[Graphic_Pipeline::Main_VERT].reset(new ShaderModule("/home/mocheng/project/MCRT/example/raster/shader/raster.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Main_FRAG].reset(new ShaderModule("/home/mocheng/project/MCRT/example/raster/shader/raster.frag.spv"));
        graphic_shader_modules[Graphic_Pipeline::Skybox_VERT].reset(new ShaderModule("/home/mocheng/project/MCRT/example/raster/shader/skybox.vert.spv"));
        graphic_shader_modules[Graphic_Pipeline::Skybox_FRAG].reset(new ShaderModule("/home/mocheng/project/MCRT/example/raster/shader/skybox.frag.spv"));
        graphic_context->prepare();
        graphic_context->prepare_descriptorset([&]() {
            auto descriptor_manager = graphic_context->get_descriptor_manager();
            //            Descriptor_Manager::Get_Singleton()
            //                ->Make_DescriptorSet(
            //                    camera_matrix,
            //                    (int)Graphic_Binding::e_camera_matrix,
            //                    Descriptor_Manager::Graphic);

            descriptor_manager->Make_DescriptorSet(
                camera_matrix,
                (int)Graphic_Binding::e_camera_matrix,
                DescriptorManager::Graphic);

            //            Descriptor_Manager::Get_Singleton()
            //                ->Make_DescriptorSet(
            //                    sky_box->get_handle(),
            //                    Descriptor_Manager::Graphic,
            //                    (int)Graphic_Binding::e_skybox,
            //                    vk::DescriptorType::eCombinedImageSampler,
            //                    vk::ShaderStageFlagBits::eFragment);
            descriptor_manager
                ->Make_DescriptorSet(
                    sky_box->get_handle(),
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_skybox,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);

            //            Descriptor_Manager::Get_Singleton()
            //                ->Make_DescriptorSet(
            //                    Texture::get_image_handles(),
            //                    Descriptor_Manager::Graphic,
            //                    (int)Graphic_Binding::e_textures,
            //                    vk::DescriptorType::eCombinedImageSampler,
            //                    vk::ShaderStageFlagBits::eFragment);
            descriptor_manager
                ->Make_DescriptorSet(
                    Texture::get_image_handles(),
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_textures,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
            //            Descriptor_Manager::Get_Singleton()
            //                ->Make_DescriptorSet(
            //                    std::vector { IBLManager::Get_Singleton()->get_LUT() },
            //                    Descriptor_Manager::Graphic,
            //                    (int)Graphic_Binding::e_LUT_image,
            //                    vk::DescriptorType::eCombinedImageSampler,
            //                    vk::ShaderStageFlagBits::eFragment);
            descriptor_manager
                ->Make_DescriptorSet(
                    std::vector { IBLManager::Get_Singleton()->get_LUT() },
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_LUT_image,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
            //            Descriptor_Manager::Get_Singleton()
            //                ->Make_DescriptorSet(
            //                    std::vector { IBLManager::Get_Singleton()->get_irradiance()->get_handle() },
            //                    Descriptor_Manager::Graphic,
            //                    (int)Graphic_Binding::e_irradiance_image,
            //                    vk::DescriptorType::eCombinedImageSampler,
            //                    vk::ShaderStageFlagBits::eFragment);
            descriptor_manager
                ->Make_DescriptorSet(
                    std::vector { IBLManager::Get_Singleton()->get_irradiance()->get_handle() },
                    DescriptorManager::Graphic,
                    (int)Graphic_Binding::e_irradiance_image,
                    vk::DescriptorType::eCombinedImageSampler,
                    vk::ShaderStageFlagBits::eFragment);
        });
        graphic_context->prepare_pipeline(graphic_shader_modules, { graphic_context->get_descriptor_manager()->get_DescriptorSet(DescriptorManager::Graphic) }, sizeof(PC_Raster));

        graphic_context->post_prepare();
    }

    {
        IBLManager::Get_Singleton()->pre_compute_irradiance();
        IBLManager::Get_Singleton()->pre_compute_LUT();
        ImageWriter::WriteImage(IBLManager::Get_Singleton()->get_LUT());
        ImageWriter::WriteCubemap(IBLManager::Get_Singleton()->get_irradiance()->get_handle());
    }
    {
        // test
//        TAA_Manager::Get_Singleton()->Init(IBLManager::Get_Singleton()->get_LUT());
    }
}

std::shared_ptr<CommandBuffer> raster_context_pbr::Begin_Frame()
{
//    TAA_Manager::Get_Singleton()->TAA_Pass();

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

std::shared_ptr<CommandBuffer> raster_context_pbr::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto render_context = std::reinterpret_pointer_cast<GraphicPass>(PASS[Graphic]);

    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {

        auto res = Context::Get_Singleton()
                       ->get_camera()
                       ->Get_v_matrix();
        res[3] = { 0, 0, 0, 1 };
        SkyboxPass(cmd,
                   render_context,
                   [&](std::shared_ptr<CommandBuffer> cmd) {
                       render_context->record_command(cmd);
                   });

        {
            cmd->get_handle()
                .bindPipeline(vk::PipelineBindPoint::eGraphics,
                              render_context->get_pipeline()->get_handle());
            render_context->record_command(cmd);

            for (auto& mesh : Mesh::meshs) {
                cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                     render_context->get_pipeline()->get_layout(),
                                                     0,
                                                     { get_graphic_context()->get_descriptor_manager()->get_DescriptorSet(DescriptorManager::Graphic)->get_handle() },
                                                     {});

                cmd->get_handle().bindIndexBuffer(mesh->get_indices_buffer()->get_handle(),
                                                  0,
                                                  vk::IndexType::eUint32);
                cmd->get_handle().bindVertexBuffers(0,
                                                    {
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
    auto& m_render_context = PASS[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}

}
