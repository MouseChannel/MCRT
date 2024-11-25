#include "Wrapper/Pipeline/Pipeline_base.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
namespace MCRT {

 
void Pipeline_base::Make_Layout(std::vector<vk::DescriptorSetLayout> descriptorSetLayout, int pc_size, vk::ShaderStageFlags push_stage)
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
void Pipeline_base::Make_Layout(std::initializer_list<std::shared_ptr<DescriptorSet>> descriptorSets, int pc_size, vk::ShaderStageFlags push_stage)
{
    std::vector<vk::DescriptorSetLayout> layouts;
    for (auto i : descriptorSets) {
        auto layout = i->get_layout();

        layouts.push_back(i->get_layout());
    }
    Make_Layout(layouts, pc_size, push_stage);
}
}