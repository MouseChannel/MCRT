#include "include/Wrapper/SubPass/BaseSubPass.hpp"

namespace MCRT {
void BaseSubPass::prepare_descriptorset(std::function<void()> prepare)
{
    prepare();
    m_descriptorManager->CreateDescriptorPool(DescriptorManager::Graphic);
    m_descriptorManager
        ->update_descriptor_set(DescriptorManager::Graphic);
}
BaseSubPass::BaseSubPass(std::shared_ptr<DescriptorManager> descriptorManager)
    : m_descriptorManager(descriptorManager)
{
}
}