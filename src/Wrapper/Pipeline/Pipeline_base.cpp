#include "Wrapper/Pipeline/Pipeline_base.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
namespace MCRT {

// Pipeline_base::Pipeline_base(std::vector<std::shared_ptr<DescriptorSet>> sets)
// {
// for (auto set : sets) {
//     descriptor_sets.push_back(set->get_handle()[0]);
//     m_descriptor_layouts.push_back(set->get_layout());
// }
// }

// Pipeline_base::Pipeline_base(std::shared_ptr<DescriptorSet> set)
// {
//     for (auto set : sets) {
//         descriptor_sets.push_back(set->get_handle()[0]);
//         m_descriptor_layouts.push_back(set->get_layout());
//     }
// }

void Pipeline_base::Make_Layout(vk::DescriptorSetLayout descriptorSetLayout, int pc_size, vk::ShaderStageFlags push_stage)
{
    auto pcrange = vk::PushConstantRange()
                       .setSize(pc_size)
                       .setStageFlags(push_stage);
    vk::PipelineLayoutCreateInfo layout_create_info;
    layout_create_info.setSetLayouts(descriptorSetLayout)
        .setPushConstantRanges(pcrange);

    pipelineLayout = Context::Get_Singleton()
                         ->get_device()
                         ->get_handle()
                         .createPipelineLayout(layout_create_info);
}

}