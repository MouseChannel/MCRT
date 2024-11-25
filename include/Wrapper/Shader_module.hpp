#pragma once

// #include <vulkan/vulkan.hpp>
#include "Wrapper/Component.hpp"
namespace MCRT {
class ShaderModule : public Component<vk::ShaderModule, ShaderModule> {
public:
    //    ShaderModule() = default;
    ShaderModule(std::string path);
    ShaderModule(std::string path, vk::ShaderStageFlagBits stage);
    void Set_SpecializationInfo(vk::SpecializationInfo m_info);
    ~ShaderModule();
    auto& get_specialinfo()
    {
        return m_info;
    }
    auto get_stage()
    {
        return m_stage;
    }

private:
    vk::ShaderStageFlagBits m_stage;
    std::vector<char> ReadBinary(const std::string& fileName);
    vk::SpecializationInfo m_info;
};
}