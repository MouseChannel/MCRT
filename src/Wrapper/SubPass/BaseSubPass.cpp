#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "Helper/DescriptorSetTarget/DescriptorSetTargetBase.hpp"
#include "Rendering/GraphicContext.hpp"
#include "Wrapper/DescriptorPool.hpp"
namespace MCRT {
BaseSubPass::BaseSubPass(std::weak_ptr<GraphicContext> graphicContext)
    : m_graphicContext(graphicContext)
{
}

void BaseSubPass::link_renderTarget(std::vector<int> color_renderTargets, std::vector<int> depth_renderTargets, std::vector<int> input_renderTargets, std::vector<int> resolve_renderTargets)
{
    //    references.resize(renderTargets.size());
    color_references.resize(color_renderTargets.size());
    depth_references.resize(depth_renderTargets.size());
    input_references.resize(input_renderTargets.size());
    resolve_references.resize(resolve_renderTargets.size());
    auto m_graphicContextp = m_graphicContext.lock();
    if (m_graphicContextp) {
        for (int i = 0; i < color_references.size(); i++) {
            color_references[i]
                .setAttachment(color_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[color_renderTargets[i]]->get_Baselayout());
            //    ->Get_Image()
            //    ->Get_image_layout());
        }
        for (int i = 0; i < depth_references.size(); i++) {
            depth_references[i]
                .setAttachment(depth_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[depth_renderTargets[i]]->get_Baselayout());
            //    ->Get_Image()
            //    ->Get_image_layout());
        }
        for (int i = 0; i < input_references.size(); i++) {
            input_references[i]
                .setAttachment(input_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[input_renderTargets[i]]->get_inputLayout());
            //    ->Get_Image()
            //    ->Get_image_layout());
        }
        for (int i = 0; i < resolve_references.size(); i++) {
            resolve_references[i]
                .setAttachment(resolve_renderTargets[i])
                .setLayout(m_graphicContextp->Get_render_targets()[resolve_renderTargets[i]]->get_Baselayout());
            //    ->Get_Image()
            //    ->Get_image_layout());
        }
    }
    description.setColorAttachments(color_references)
        .setPDepthStencilAttachment(depth_references.data())
        // .setResolveAttachments(resolve_references)
        .setInputAttachments(input_references);
    if (resolve_references.size() > 0) {
        description.setResolveAttachments(resolve_references);
    }
}
void BaseSubPass::set_subpassDependency(vk::SubpassDependency dependency)
{
    dependencies.push_back(dependency);
}
void BaseSubPass::set_subpassDescription(vk::SubpassDescription des)
{
    //    description.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
}
// void BaseSubPass::AddDescriptorSetTarget(std::shared_ptr<DescriptorSetTargetBase> target)
// {
//     m_descriptorSetTarget.push_back(target);
//     m_descriptorSet->Add(target);
// }
// // void BaseSubPass::AddDescriptorSetTarget(std::vector<std::shared_ptr<DescriptorSetTargetBase>> targets)
// // {
// //     m_descriptorSetTarget.push_back(target);
// //     m_descriptorSet->Add(target);
// // }
// void BaseSubPass::Prepare_DescriptorSet(std::function<void()> prepare)
// {
//     if (m_descriptorSet == nullptr)
//         m_descriptorSet.reset(new DescriptorSet());
//     prepare();
//     if (m_descriptorSet->check_dirty()) {

//         std::map<vk::DescriptorType, uint32_t> type_map;

//         for (auto& i : m_descriptorSetTarget) {
//             type_map[i->get_type()]++;
//         }

//         std::vector<vk::DescriptorPoolSize> pool_infos(type_map.size());
//         std::transform(type_map.begin(), type_map.end(), pool_infos.begin(), [](auto item) {
//             return vk::DescriptorPoolSize()
//                 .setDescriptorCount(item.second)
//                 .setType(item.first);
//         });

//         m_descriptorSetPool.reset(new DescriptorPool(pool_infos));
//         // allocate descriptorset

//         m_descriptorSet->create(m_descriptorSetPool, get_DescriptorSetCount());
//     }
//     for (auto& i : m_descriptorSetTarget) {
//         i->Update();
//     }
// }
int BaseSubPass::get_DescriptorSetCount()
{
    throw std::runtime_error("has not set descriptorset count!!");
}

}