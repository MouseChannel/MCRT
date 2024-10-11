#include "Wrapper/SubPass/GbufferSubPass.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Wrapper/Shader_module.hpp"
// #include "Context/raster_context.hpp"
// #include "Shader/Data_struct.h"
namespace MCRT {
using Shader_Stage = Graphic_Pipeline::Shader_Stage;
GbufferSubPass::GbufferSubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index, std::vector<std::shared_ptr<GBuffer_RenderTarget>> render_Targets)
    : m_render_Targets(render_Targets)
    , BaseSubPass(graphicContext, subpass_index)
{
}
GbufferSubPass::GbufferSubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index)
    : BaseSubPass(graphicContext, subpass_index)
{
}
void GbufferSubPass::prepare_pipeline(int pc_size)
{
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        // m_pipeline.reset(new Graphic_Pipeline(shaders, { m_graphicContextp->get_descriptor_manager()->get_DescriptorSet(DescriptorManager::Graphic) }, pc_size));
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
        m_pipeline->Make_Resterization(vk::CullModeFlagBits::eBack);
        m_pipeline->Make_Subpass_index(m_subpass_index);
        m_pipeline->Make_OpacityAttach(color_references.size());
        m_pipeline->Make_DepthTest(true,false);
        m_pipeline->Make_Blend();
        m_pipeline->Make_Layout(m_descriptorSet->get_layout(),
                                sizeof(PC_Raster),
                                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    }
}
void GbufferSubPass::prepare_vert_shader_module(std::string _vert_shader)
{
    shaders[(int)Shader_Stage::VERT].reset(new ShaderModule(_vert_shader));
}
void GbufferSubPass::prepare_frag_shader_module(std::string _frag_shader)
{
    shaders[(int)Shader_Stage::FRAG].reset(new ShaderModule(_frag_shader));
}
void GbufferSubPass::post_prepare()
{
    m_pipeline->Build_Pipeline(
        Context::Get_Singleton()
            ->get_graphic_context()
            ->Get_render_pass());
}
void GbufferSubPass::recreate()
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
