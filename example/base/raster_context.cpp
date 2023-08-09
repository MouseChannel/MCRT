#include "example/base/raster_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/CommandBuffer.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"

namespace MCRT {
raster_context::raster_context()
{
}
raster_context::~raster_context()
{
}
void raster_context::prepare(std::shared_ptr<Window> window)
{
    Context::prepare(window);
    Camera_matrix _camera_data {
        .view = m_camera->Get_v_matrix(),
        .project = m_camera->Get_p_matrix()
    };
    camera_matrix = UniformManager::make_uniform({ _camera_data },
                                                 vk::ShaderStageFlagBits::eVertex,
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

void raster_context::re_create_context(){}
// std::shared_ptr<CommandBuffer> raster_context::BeginGraphicFrame()
// {

//     auto& render_context = contexts[Graphic];
//     std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
//     {
//         cmd->get_handle().bindPipeline(vk::PipelineBindPoint ::eGraphics, render_context->get_pipeline()->get_handle());
//         cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
//                                              render_context->get_pipeline()->get_layout(),
//                                              0,
//                                              { Descriptor_Manager::Get_Singleton()
//                                                    ->get_DescriptorSet(Descriptor_Manager::Graphic)
//                                                    ->get_handle() },
//                                              {});
//         for (auto& mesh : Mesh::meshs) {

//             cmd->get_handle().bindIndexBuffer(mesh->get_indices_buffer()->get_handle(), 0, vk::IndexType ::eUint32);
//             cmd->get_handle().bindVertexBuffers(0, {
//                                                        mesh->get_vertex_buffer()->get_handle(),
//                                                    },
//                                                 { 0, 0 });
//             cmd->get_handle()
//                 .drawIndexed(mesh->get_vertex_count(),
//                              1,
//                              0,
//                              0,
//                              0);
//         }
//         render_context->record_command(cmd);
//     }

//     return cmd;
// }

// void raster_context::EndGraphicFrame()
// {
//     auto& m_render_context = contexts[Graphic];
//     m_render_context->Submit();
//     m_render_context->EndFrame();
// }

}