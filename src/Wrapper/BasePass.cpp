#include "Wrapper/BasePass.hpp"
#include "Wrapper/DescriptorPool.hpp"
namespace MCRT {

void BasePass::Prepare_DescriptorSet(std::function<void()> prepare)
{
    if (m_descriptorSet == nullptr)
        m_descriptorSet.reset(new DescriptorSet());
    prepare();
 
    if (m_descriptorSet->check_dirty()) {

        std::map<vk::DescriptorType, uint32_t> type_map;

        // for (auto& i : m_descriptorSetTarget) {
        //     type_map[i->get_type()]++;
        // }
        for (auto& [key, value] : m_descriptorSetTargetMap) {
            type_map[value->get_type()]++;
        }

        std::vector<vk::DescriptorPoolSize> pool_infos(type_map.size());
        std::transform(type_map.begin(), type_map.end(), pool_infos.begin(), [](auto item) {
            return vk::DescriptorPoolSize()
                .setDescriptorCount(item.second)
                .setType(item.first);
        });

        m_descriptorSetPool.reset(new DescriptorPool(pool_infos));
        // allocate descriptorset

        m_descriptorSet->build(m_descriptorSetPool, get_DescriptorSetCount());
    }
    for (auto& [k,v] : m_descriptorSetTargetMap) {
        v->Update();
    }
}
void BasePass::AddDescriptorSetTarget(std::shared_ptr<DescriptorSetTargetBase> target)
{
    // m_descriptorSetTarget.push_back(target);
    m_descriptorSetTargetMap[target->get_binding_index()] = target;
    m_descriptorSet->Add(target);
}
int BasePass::get_DescriptorSetCount()
{
    throw std::runtime_error("has not set descriptorset count!!");
}

}