#include "Wrapper/ComputePass/ComputePass.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "shaders/PBR/IBL/push_constants.h"
namespace MCRT {
ComputePass::ComputePass()
{
}
void ComputePass::SetShaderModule(std::string shader_path)
{
    m_shader_module.reset(new ShaderModule(shader_path));
}
void ComputePass::SetShaderModule(std::shared_ptr<ShaderModule> shader_module){
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

 
}