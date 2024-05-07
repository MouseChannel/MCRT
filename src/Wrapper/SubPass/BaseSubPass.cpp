#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "Rendering/GraphicContext.hpp"
namespace MCRT {
BaseSubPass::BaseSubPass(std::weak_ptr<GraphicContext> graphicContext)
    : m_graphicContext(graphicContext)
{
}
// void BaseSubPass::prepare_descriptorset(std::function<void()> prepare)
//{
//     prepare();
//     //    auto m_graphicContextp = m_graphicContext.lock();
//     //    if (m_graphicContextp) {
//     //
//     //        m_graphicContextp->get_descriptor_manager()
//     //            ->CreateDescriptorPool(DescriptorManager::Graphic);
//     //        m_graphicContextp->get_descriptor_manager()
//     //            ->update_descriptor_set(DescriptorManager::Graphic);
//     //    }
// }
void BaseSubPass::link_renderTarget(std::vector<int> color_renderTargets, std::vector<int> depth_renderTargets, std::vector<int> input_renderTargets)
{
    //    references.resize(renderTargets.size());
    color_references.resize(color_renderTargets.size());
    depth_references.resize(depth_renderTargets.size());
    input_references.resize(input_renderTargets.size());
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        for (int i = 0; i < color_references.size(); i++) {
            color_references[i]
                .setAttachment(color_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[color_renderTargets[i]]
                               ->Get_Image()
                               ->Get_image_layout());
        }
        for (int i = 0; i < depth_references.size(); i++) {
            depth_references[i]
                .setAttachment(depth_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[depth_renderTargets[i]]
                               ->Get_Image()
                               ->Get_image_layout());
        }
        for (int i = 0; i < input_references.size(); i++) {
            input_references[i]
                .setAttachment(input_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[input_renderTargets[i]]
                               ->Get_Image()
                               ->Get_image_layout());
        }
    }
    description.setColorAttachments(color_references)
        .setPDepthStencilAttachment(depth_references.data())
        .setInputAttachments(input_references);
}
void BaseSubPass::set_subpassDependency(vk::SubpassDependency dependency)
{
    dependencies.push_back(dependency);
}
void BaseSubPass::set_subpassDescription(vk::SubpassDescription des)
{
    //    description.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
}
 
}