#include "Wrapper/SubPass/ToneMapSubpass.hpp"
#include "Helper/DescriptorSetTarget/ImageDescriptorTarget.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Rendering/Model.hpp"
#include "Wrapper/Shader_module.hpp"
#include "example/base/raster_context.hpp"
#include "example/raster/raster_pbr_context.hpp"
#include "example/raster/shader/Binding.h"
#include "shaders/Data_struct.h"
namespace MCRT {
using Shader_Stage = Graphic_Pipeline::Shader_Stage;
ToneMapSubPass::ToneMapSubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index)
    : BaseSubPass(graphicContext,subpass_index)
{
}
void ToneMapSubPass::make_offscreen_mesh()
{
}
void ToneMapSubPass::prepare_vert_shader_module(std::string _vert_shader)
{
    shaders[(int)Shader_Stage::VERT].reset(new ShaderModule(_vert_shader));
}
void ToneMapSubPass::prepare_frag_shader_module(std::string _frag_shader)
{
    shaders[(int)Shader_Stage::FRAG].reset(new ShaderModule(_frag_shader));
}
void ToneMapSubPass::prepare_pipeline(int pc_size)
{
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        // m_pipeline.reset(new Graphic_Pipeline(shaders,
        //                                       { m_graphicContextp
        //                                             ->get_descriptor_manager()
        //                                             ->get_DescriptorSet(DescriptorManager::Graphic) },
        //                                       pc_size));

        m_pipeline.reset(new Graphic_Pipeline(shaders,
                                              { m_descriptorSet },
                                              pc_size));

        auto binds = Vertex::make_bind();
        auto attrs = Vertex::make_attr();

        m_pipeline->Make_VertexInput(binds, attrs);
        m_pipeline->Make_VertexAssembly();
        m_pipeline->Make_viewPort();
        m_pipeline->Make_MultiSample(m_graphicContextp
                                         ->Get_render_targets()[color_references[0].attachment]
                                         ->Get_attachment_description()
                                         .samples);
        m_pipeline->Make_Resterization();
        m_pipeline->Make_Subpass_index(m_subpass_index);

        m_pipeline->Make_OpacityAttach(color_references.size());
        m_pipeline->Make_DepthTest();
        m_pipeline->Make_Blend();
        m_pipeline->Make_Layout(m_descriptorSet->get_layout(), pc_size, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex);
    }
}
void ToneMapSubPass::post_prepare()
{
    m_pipeline->Build_Pipeline(
        Context::Get_Singleton()
            ->get_graphic_context()
            ->Get_render_pass());
}
void ToneMapSubPass::recreate()
{

    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {

        Prepare_DescriptorSet([&]() {
            for (int i = 0; i < get_DescriptorSetCount(); i++) {
                auto input_renderTarget = m_graphicContextp->Get_render_targets(i)[m_graphicContextp->resolveAttachmentindex];
                AddDescriptorTarget(std::make_shared<ImageDescriptorTarget>(
                    // IBLManager::Get_Singleton()->get_skybox(),
                    std::vector { input_renderTarget->Get_Image()->Get_Image_View() },
                    std::vector { input_renderTarget->get_inputLayout() },
                    // Which_Set::Graphic,
                    (int)Graphic_Binding::e_tonemap_input,
                    vk::ShaderStageFlagBits::eFragment,
                    vk::DescriptorType::eInputAttachment,
                    get_DescriptorSet(),
                    i));
            }
        });
    }
    prepare_pipeline(sizeof(PC_Raster));
    post_prepare();
}
}