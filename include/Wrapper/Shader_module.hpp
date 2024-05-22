#pragma once

// #include <vulkan/vulkan.hpp>
#include "Wrapper/Component.hpp"
namespace MCRT {
class ShaderModule : public Component<vk::ShaderModule, ShaderModule> {
public:
    //    ShaderModule() = default;
    ShaderModule(std::string path);
    void Set_SpecializationInfo(vk::SpecializationInfo m_info);
    ~ShaderModule();
    auto &get_specialinfo()
    {
        return m_info;
    }

private:
    std::vector<char> ReadBinary(const std::string& fileName);
    vk::SpecializationInfo m_info;
};
}