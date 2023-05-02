#pragma once
#include <string_view>
// #include <vulkan/vulkan.hpp>
#include "Wrapper/Component.hpp"
namespace MCRT {
class ShaderModule : public Component<vk::ShaderModule, ShaderModule> {
public:
    ShaderModule(std::string path);
    ~ShaderModule();

private:
    std::vector<char> ReadBinary(const std::string& fileName);
};
}