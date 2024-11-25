#include "Wrapper/GraphicPass/GraphicPass.hpp"
namespace MCRT {

GraphicPass::GraphicPass(GraphicContext* graphic_context)
{
    m_graphicContext = graphic_context;
}

void GraphicPass::link_renderTarget(std::vector<int> color_renderTargets, std::vector<int> depth_renderTargets, std::vector<int> input_renderTargets, std::vector<int> resolve_renderTargets)
{
    color_references.resize(color_renderTargets.size());
    depth_references.resize(depth_renderTargets.size());
    input_references.resize(input_renderTargets.size());
    resolve_references.resize(resolve_renderTargets.size());

    for (int i = 0; i < color_references.size(); i++) {
        color_references[i]
            .setAttachment(color_renderTargets[i])
            .setLayout(m_graphicContext->Get_render_targets()[color_renderTargets[i]]->get_Baselayout());
 
    }
    for (int i = 0; i < depth_references.size(); i++) {
        depth_references[i]
            .setAttachment(depth_renderTargets[i])
            .setLayout(m_graphicContext->Get_render_targets()[depth_renderTargets[i]]->get_Baselayout());
 
    }
    for (int i = 0; i < input_references.size(); i++) {
        input_references[i]
            .setAttachment(input_renderTargets[i])
            .setLayout(m_graphicContext->Get_render_targets()[input_renderTargets[i]]->get_inputLayout());
 
    }
    for (int i = 0; i < resolve_references.size(); i++) {
        resolve_references[i]
            .setAttachment(resolve_renderTargets[i])
            .setLayout(m_graphicContext->Get_render_targets()[resolve_renderTargets[i]]->get_Baselayout());
 
    }

    description.setColorAttachments(color_references)

        .setPDepthStencilAttachment(depth_references.data())
        // .setResolveAttachments(resolve_references)
        .setInputAttachments(input_references);
    if (resolve_references.size() > 0) {
        description.setResolveAttachments(resolve_references);
    }
}

}