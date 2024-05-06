#include "Wrapper/SubPass/SkyboxSubPass.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shaders/Data_struct.h"
namespace MCRT {
using Shader_Stage = Graphic_Pipeline::Shader_Stage;
SkyboxSubPass::SkyboxSubPass(std::shared_ptr<DescriptorManager> descriptorManager)
    : BaseSubPass(descriptorManager)
{
}

void SkyboxSubPass::run_subpass()
{
}
void SkyboxSubPass::set_description()
{
    vk::AttachmentReference reference;
//    reference.setAttachment()
//    description.setColorAttachments()
}

void SkyboxSubPass::prepare_pipeline()
{
    m_pipeline.reset(new Graphic_Pipeline(shaders, { m_descriptorManager->get_DescriptorSet(DescriptorManager::Graphic) }, sizeof(int)));

    auto binds = Vertex::make_bind();
    auto attrs = Vertex::make_attr();

    m_pipeline->Make_VertexInput(binds, attrs);
    m_pipeline->Make_VertexAssembly();
    m_pipeline->Make_viewPort();
    m_pipeline->Make_MultiSample();
    m_pipeline->Make_Resterization();
    m_pipeline->Make_attach();
    m_pipeline->Make_OpacityAttach();
    m_pipeline->Make_DepthTest();

    m_pipeline->Build_Pipeline(
        Context::Get_Singleton()
            ->get_graphic_context()
            ->Get_render_pass());
}
void SkyboxSubPass::prepare_vert_shader_module(std::string _vert_shader)
{
    shaders[(int)Shader_Stage::VERT].reset(new ShaderModule(_vert_shader));
}
void SkyboxSubPass::prepare_frag_shader_module(std::string _frag_shader)
{
    shaders[(int)Shader_Stage::FRAG].reset(new ShaderModule(_frag_shader));
}

}
