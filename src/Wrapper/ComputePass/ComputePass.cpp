#include "Wrapper/ComputePass/ComputePass.hpp"
#include "Shader/PBR/IBL/push_constants.h"
namespace MCRT {
ComputePass::ComputePass()
{
}
void ComputePass::SetShaderModule(std::string shader_path)
{
    m_shader_module.reset(new ShaderModule(shader_path));
}
void ComputePass::SetShaderModule(std::shared_ptr<ShaderModule> shader_module)
{
    m_shader_module = shader_module;
}
void ComputePass::prepare_pipeline(int pc_size)
{
    m_pipeline.reset(new Compute_Pipeline(m_shader_module));
    m_pipeline->Make_Layout(m_descriptorSet->get_layout(), pc_size, vk::ShaderStageFlagBits::eCompute);
    m_pipeline->Build();
}
void ComputePass::Dispach(vk::CommandBuffer cmd, int local_size_x, int local_size_y, int local_size_z)
{
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                           m_pipeline->get_layout(),
                           0,
                           m_descriptorSet->get_handle(),
                           {});
    cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                     m_pipeline->get_handle());

    cmd.dispatch(local_size_x, local_size_y, local_size_z);
}

// void ComputePass::PreparePipeline()
// {
//     m_pipeline.reset(new Compute_Pipeline(m_shader_module));
//     m_pipeline->Make_Layout(m_descriptorSet->get_layout(), sizeof(PushContant_IBL), vk::ShaderStageFlagBits::eCompute);
//     m_pipeline->Build();
// }
// void ComputePass::PrepareDescriptorSet(std::function<void()> prepare)
// {
//      if (m_descriptorSet == nullptr)
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

}