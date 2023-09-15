#include "Wrapper/Pipeline/Pipeline_base.hpp"
#include "Wrapper/DescriptorSet.hpp"
namespace MCRT {

Pipeline_base::Pipeline_base(std::vector<std::shared_ptr<DescriptorSet>> sets)
{
    for (auto set : sets) {
        descriptor_sets.push_back(set->get_handle()[0]);
        m_descriptor_layouts.push_back(set->get_layout());
    }
}

}