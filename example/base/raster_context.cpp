#include "example/base/raster_context.hpp"
#include "Helper/Camera.hpp"
// #include "Helper/DescriptorManager.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "example/raster/shader/Constants.h"

namespace MCRT {
raster_context::raster_context()
{
}
raster_context::~raster_context()
{
    camera_matrix.reset();
}
void raster_context::prepare(std::shared_ptr<Window> window)
{
    Context::prepare(window);
    Camera_matrix _camera_data {
        .view = m_camera->Get_v_matrix(),
        .project = m_camera->Get_p_matrix()
    };
    camera_matrix = UniformManager::make_uniform({ _camera_data },
                                                 vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                                                 vk::DescriptorType ::eUniformBuffer);
}
std::shared_ptr<CommandBuffer> raster_context::Begin_Frame()
{
    m_camera->move_update();

    return BeginGraphicFrame();
}
void raster_context::EndFrame()
{
    EndGraphicFrame();
}

void raster_context::re_create_context()
{
    // auto graphic_context = std::reinterpret_pointer_cast<GraphicContext>(PASS[Graphic]);
    // for(int )
    // graphic_context->subpasses
}

// void raster_context::SkyboxPass(std::shared_ptr<CommandBuffer> cmd, std::shared_ptr<GraphicContext> graphic_context, std::function<void(std::shared_ptr<CommandBuffer> cmd)> func)
//{
//     cmd->get_handle()
//         .bindPipeline(vk::PipelineBindPoint::eGraphics,
//                       get_skybox_pipeline()->get_handle());
//
//     cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
//                                          get_skybox_pipeline()->get_layout(),
//                                          0,
//                                          { graphic_context->get_descriptor_manager()
//                                                ->get_DescriptorSet(DescriptorManager::Graphic)
//                                                ->get_handle() },
//                                      nullptr);
//
//     cmd->get_handle().bindIndexBuffer(get_skybox_mesh()->get_indices_buffer()->get_handle(),
//                                       0,
//                                       vk::IndexType ::eUint32);
//     cmd->get_handle().bindVertexBuffers(0, {
//                                                get_skybox_mesh()->get_vertex_buffer()->get_handle(),
//                                            },
//                                         { 0 });
//     // auto res = Context::Get_Singleton()
//     //                ->get_camera()
//     //                ->Get_v_matrix();
//     // res[3] = { 0, 0, 0, 1 };
//
//     cmd->get_handle()
//         .pushConstants<PC_Raster>(
//             get_skybox_pipeline()->get_layout(),
//             vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
//             0,
//             PC_Raster {
//
////                .view_matrix { Context::Get_Singleton()
////                                   ->get_camera()
////                                   ->Get_v_matrix() },
//                .color_texture_index = get_skybox_mesh()->m_material.color_texture_index,
//                .light_pos { 0, 0, 5, 1 },
//            });
//
//    func(cmd);
//
//    cmd->get_handle()
//        .drawIndexed(get_skybox_mesh()->get_vertex_count(),
//                     1,
//                     0,
//                     0,
//                     0);
//}

}