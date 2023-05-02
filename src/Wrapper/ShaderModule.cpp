#include "Wrapper/Device.hpp"
#include "Wrapper/Shader_module.hpp"
#include <fstream>

// #include <iosfwd>
namespace MCRT {
std::vector<char> ShaderModule::ReadBinary(const std::string& fileName)
{
    std::ifstream file(fileName.c_str(),
                       std::ios::ate | std::ios::binary | std::ios::in);

    if (!file) {
        throw std::runtime_error("Error: failed to open shader file");
    }

    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
ShaderModule::ShaderModule(std::string path)
{
    auto source = ReadBinary(path);
    vk::ShaderModuleCreateInfo createInfo;
    createInfo
        .setCodeSize(source.size())

        .setPCode((const uint32_t*)source.data());

    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createShaderModule(createInfo);
}
ShaderModule::~ShaderModule()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroyShaderModule(m_handle);
}
}