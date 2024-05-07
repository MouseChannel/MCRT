#include "Wrapper/SubPass/SkyboxSubPass.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shaders/Data_struct.h"
namespace MCRT {
using Shader_Stage = Graphic_Pipeline::Shader_Stage;
SkyboxSubPass::SkyboxSubPass(std::weak_ptr<GraphicContext> graphicContext)
    : BaseSubPass(graphicContext)
{
}
 
void SkyboxSubPass::prepare_pipeline(int pc_size)
{
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        m_pipeline.reset(new Graphic_Pipeline(shaders, { m_graphicContextp->get_descriptor_manager()->get_DescriptorSet(DescriptorManager::Graphic) }, pc_size));

        auto binds = Vertex::make_bind();
        auto attrs = Vertex::make_attr();

        m_pipeline->Make_VertexInput(binds, attrs);
        m_pipeline->Make_VertexAssembly();
        m_pipeline->Make_viewPort();
        m_pipeline->Make_MultiSample();
        m_pipeline->Make_Resterization();
        m_pipeline->Make_Subpass_index(subpass_index);
        m_pipeline->Make_OpacityAttach();
        m_pipeline->Make_DepthTest();
        m_pipeline->Make_Blend();

 
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

}
