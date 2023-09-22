
#include "Wrapper/ShaderModule.hpp"
#include "Rendering/Context.hpp"
#include "Wrapper/Device.hpp"
#include "cstring"
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include <android/asset_manager.h>

#include <android_native_app_glue.h>
#endif
#include <cstdint>
#include <fstream>
#include <iosfwd>
#include <string>
#include <vulkan/vulkan_structs.hpp>

namespace MoCheng3D {

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
ShaderModule::ShaderModule(const std::string path)
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto m_app = Context::Get_Singleton()->Get_app();
    AAsset* file = AAssetManager_open(m_app->activity->assetManager,
        path.c_str(), AASSET_MODE_BUFFER);

    size_t fileLength = AAsset_getLength(file);

    std::vector<char> source;
    source.resize(fileLength);

    AAsset_read(file, source.data(), fileLength);
    AAsset_close(file);

#else
    auto source = ReadBinary(path);
#endif
    vk::ShaderModuleCreateInfo createInfo;
    createInfo
        .setCodeSize(source.size())
        .setPCode((const uint32_t*)source.data());

    m_handle = Get_Context_Singleton()
                   ->Get_Device()
                   ->Get_handle()
                   .createShaderModule(createInfo);
}
ShaderModule::~ShaderModule()
{
    Get_Context_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroyShaderModule(m_handle);
}
} // namespace MoCheng3D