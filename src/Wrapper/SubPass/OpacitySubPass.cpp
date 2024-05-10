#include "Wrapper/SubPass/OpacitySubPass.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shaders/Data_struct.h"
#include "example/base/raster_context.hpp"
namespace MCRT {
using Shader_Stage = Graphic_Pipeline::Shader_Stage;
OpacitySubPass::OpacitySubPass(std::weak_ptr<GraphicContext> graphicContext)
    : BaseSubPass(graphicContext)
{
}
void OpacitySubPass::prepare_vert_shader_module(std::string _vert_shader)
{
    shaders[(int)Shader_Stage::VERT].reset(new ShaderModule(_vert_shader));
}
void OpacitySubPass::prepare_frag_shader_module(std::string _frag_shader)
{
    shaders[(int)Shader_Stage::FRAG].reset(new ShaderModule(_frag_shader));
}
void OpacitySubPass::prepare_pipeline(int pc_size)
{
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        m_pipeline.reset(new Graphic_Pipeline(shaders,
                                              { m_graphicContextp
                                                    ->get_descriptor_manager()
                                                    ->get_DescriptorSet(DescriptorManager::Graphic) },
                                              pc_size));

        auto binds = Vertex::make_bind();
        auto attrs = Vertex::make_attr();

        m_pipeline->Make_VertexInput(binds, attrs);
        m_pipeline->Make_VertexAssembly();
        m_pipeline->Make_viewPort();
        m_pipeline->Make_MultiSample();
        m_pipeline->Make_Resterization();
        m_pipeline->Make_Subpass_index(raster_context::OpacitySubPassIndex);

        m_pipeline->Make_OpacityAttach();
        m_pipeline->Make_DepthTest();
        m_pipeline->Make_Blend();
    }
}
void OpacitySubPass::post_prepare()
{
    m_pipeline->Build_Pipeline(
        Context::Get_Singleton()
            ->get_graphic_context()
            ->Get_render_pass());
}
// void OpacitySubPass::draw(vk::CommandBuffer& cmd, std::vector<std::shared_ptr<Mesh>>& meshs, void* push_constant)
// {
//     cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline->get_handle());
//     // render_context->record_command(cmd);
//     //            for (auto& mesh : Mesh::meshs) {
//     auto graphic_context = m_graphicContext.lock();
//     if (graphic_context) {
//         cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
//                                m_pipeline->get_layout(),
//                                0,
//                                { graphic_context->get_descriptor_manager()
//                                      ->get_DescriptorSet(
//                                          DescriptorManager::Graphic)
//                                      ->get_handle() },
//                                {});
//     }

//     cmd.bindIndexBuffer(meshs[0]->get_indices_buffer()->get_handle(),
//                         0,
//                         vk::IndexType::eUint32);

//     cmd.bindVertexBuffers(0,
//                           {
//                               meshs[0]->get_vertex_buffer()->get_handle(),
//                           },
//                           { 0 });
//     cmd.pushConstants(m_pipeline->get_layout(),
//                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
//                       0,
//                       m_pipeline->get_pushconstants_size(),
//                       push_constant);

//     cmd.drawIndexed(meshs[0]->get_vertex_count(),
//                     1,
//                     0,
//                     0,
//                     0);
//     //            render_context->record_command(cmd);
// }
// void OpacitySubPass::draw1(vk::CommandBuffer& cmd)
// {
// }

}