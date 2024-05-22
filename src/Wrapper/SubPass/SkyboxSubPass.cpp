#include "Wrapper/SubPass/SkyboxSubPass.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Shader_module.hpp"
#include "example/base/raster_context.hpp"
#include "example/raster/raster_pbr_context.hpp"
#include "shaders/Data_struct.h"
namespace MCRT {
using Shader_Stage = Graphic_Pipeline::Shader_Stage;
SkyboxSubPass::SkyboxSubPass(std::weak_ptr<GraphicContext> graphicContext)
    : BaseSubPass(graphicContext)
{
    Make_SkyboxMesh();
}
void SkyboxSubPass::Make_SkyboxMesh()
{
    std::vector<Vertex> vertexs {
        { { -1, 1, 1 } },
        { { -1, -1, 1 } },
        { { -1, -1, -1 } },
        { { -1, 1, 1 } },
        { { -1, -1, -1 } },
        { { -1, 1, -1 } },
        { { -1, 1, -1 } },
        { { -1, -1, -1 } },
        { { 1, -1, -1 } },
        { { -1, 1, -1 } },
        { { 1, -1, -1 } },
        { { 1, 1, -1 } },
        { { 1, 1, -1 } },
        { { 1, -1, -1 } },
        { { 1, -1, 1 } },
        { { 1, 1, -1 } },
        { { 1, -1, 1 } },
        { { 1, 1, 1 } },
        { { 1, 1, 1 } },
        { { 1, -1, 1 } },
        { { -1, -1, 1 } },
        { { 1, 1, 1 } },
        { { -1, -1, 1 } },
        { { -1, 1, 1 } },

        { { -1, 1, -1 } },
        { { 1, 1, -1 } },
        { { 1, 1, 1 } },
        { { -1, 1, -1 } },
        { { 1, 1, 1 } },
        { { -1, 1, 1 } },
        { { 1, -1, -1 } },
        { { -1, -1, -1 } },
        { { -1, -1, 1 } },
        { { 1, -1, -1 } },
        { { -1, -1, 1 } },
        { { 1, -1, 1 } }

    };

    std::vector<uint32_t> faces(vertexs.size());
    for (int i = 0; i < vertexs.size(); i++) {
        faces[i] = i;
    }
    // std::shared_ptr<Mesh> tar { new Mesh("SKYBOX", vertexs, faces, {}) };

    m_meshs.emplace_back(new Mesh("SKYBOX", vertexs, faces, {}));
}
void SkyboxSubPass::prepare_pipeline(int pc_size)
{
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        // m_pipeline.reset(new Graphic_Pipeline(shaders, { m_graphicContextp->get_descriptor_manager()->get_DescriptorSet(DescriptorManager::Graphic) }, pc_size));

        m_pipeline.reset(new Graphic_Pipeline(shaders, { m_descriptorSet }, pc_size));

        auto binds = Vertex::make_bind();
        auto attrs = Vertex::make_attr();

        m_pipeline->Make_VertexInput(binds, attrs);
        m_pipeline->Make_VertexAssembly();
        m_pipeline->Make_viewPort();
        m_pipeline->Make_MultiSample(vk::SampleCountFlagBits::e8);
        m_pipeline->Make_Resterization(vk::CullModeFlagBits::eNone);
        m_pipeline->Make_Subpass_index(raster_context::SkyboxSubPassIndex);
        m_pipeline->Make_OpacityAttach(color_references.size());
        m_pipeline->Make_DepthTest();
        m_pipeline->Make_Blend();
        m_pipeline->Make_Layout(m_descriptorSet->get_layout(), pc_size, vk::ShaderStageFlagBits::eFragment|vk::ShaderStageFlagBits::eVertex);
    }
}
void SkyboxSubPass::prepare_vert_shader_module(std::string _vert_shader)
{
    shaders[(int)Shader_Stage::VERT].reset(new ShaderModule(_vert_shader));
}
void SkyboxSubPass::prepare_frag_shader_module(std::string _frag_shader)
{
    shaders[(int)Shader_Stage::FRAG].reset(new ShaderModule(_frag_shader));
}
void SkyboxSubPass::post_prepare()
{
    m_pipeline->Build_Pipeline(
        Context::Get_Singleton()
            ->get_graphic_context()
            ->Get_render_pass());
}
// void SkyboxSubPass::Prepare_DescriptorSet(std::function<void()> prepare)
// {
//     prepare();

//     if (m_descriptorSet->check_dirty()) {
//         for (auto i : m_descriptorSetTarget) { }
//     }
// }

// void SkyboxSubPass::draw(vk::CommandBuffer& cmd, std::vector<std::shared_ptr<Mesh>>& meshs, void* push_constant)
// {
//     for (auto& mesh : meshs) {
//         // auto& opacitySubpass = render_context->get_subpasses()[1];
//         cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,
//                          m_pipeline->get_handle());
//         // render_context->record_command(cmd);
//         //            for (auto& mesh : Mesh::meshs) {
//         auto graphic_context = m_graphicContext.lock();
//         if (graphic_context) {
//             cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
//                                    m_pipeline->get_layout(),
//                                    0,
//                                    { graphic_context->get_descriptor_manager()
//                                          ->get_DescriptorSet(
//                                              DescriptorManager::Graphic)
//                                          ->get_handle() },
//                                    {});
//         }

//         cmd.bindIndexBuffer(mesh->get_indices_buffer()->get_handle(),
//                             0,
//                             vk::IndexType::eUint32);

//         cmd.bindVertexBuffers(0,
//                               {
//                                   mesh->get_vertex_buffer()->get_handle(),
//                               },
//                               { 0 });
//         auto m = glm::mat4(1);

//         cmd.pushConstants(m_pipeline->get_layout(),
//                           vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
//                           0,
//                           m_pipeline->get_pushconstants_size(),
//                           push_constant);
//         //            render_context->record_command(cmd);
//         cmd.drawIndexed(mesh->get_vertex_count(),
//                         1,
//                         0,
//                         0,
//                         0);
//     }
// }
}
