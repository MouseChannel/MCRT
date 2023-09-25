#include "Wrapper/Device.hpp"
#include "Wrapper/Shader_module.hpp"
#include <fstream>

// #include <iosfwd>
namespace MCRT {
    std::vector<char> ShaderModule::ReadBinary(const std::string &fileName) {

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

    ShaderModule::ShaderModule(std::string path) {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        auto app = Context::Get_Singleton()->Get_app();
        AAsset *file = AAssetManager_open(app->activity->assetManager,
                                          path.c_str(), AASSET_MODE_BUFFER);
//        AAsset *file1 = AAssetManager_open(m_app->activity->assetManager,
//                                          "shaders/vert.vert", AASSET_MODE_STREAMING);
//
//        AAssetDir *dirr = AAssetManager_openDir(m_app->activity->assetManager,
//                                          "shaders" );
//        auto rr = AAssetDir_getNextFileName(dirr);

        size_t fileLength = AAsset_getLength(file);

        char *fileContent = new char[fileLength];
        std::vector<uint32_t> shader_code;
        shader_code.resize(fileLength / 4);


        AAsset_read(file, shader_code.data(), fileLength);
        AAsset_close(file);
        vk::ShaderModuleCreateInfo shader_ci;
        shader_ci.setCodeSize(fileLength)
                .setCode(shader_code);
        m_handle = Context::Get_Singleton()
                ->get_device()
                ->get_handle()
                .createShaderModule(shader_ci);


#else

        auto source = ReadBinary(path);
        vk::ShaderModuleCreateInfo createInfo;
        createInfo
            .setCodeSize(source.size())

            .setPCode((const uint32_t*)source.data());

        m_handle = Get_Context_Singleton()
                       ->get_device()
                       ->get_handle()
                       .createShaderModule(createInfo);
#endif
    }

    ShaderModule::~ShaderModule() {
        Get_Context_Singleton()
                ->get_device()
                ->get_handle()
                .destroyShaderModule(m_handle);
    }
}