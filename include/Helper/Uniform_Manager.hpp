#pragma once
#include "Helper/Debugger.hpp"
#include "Wrapper/Buffer.hpp"
#include <vulkan/vulkan.hpp>
#include <typeinfo>

namespace MCRT {
template <typename T>
class Uniform_Stuff {
public:
    Uniform_Stuff() = default;
    Uniform_Stuff(std::vector<T> t, vk::ShaderStageFlags shader_stage, vk::DescriptorType type)

        : data(t)
        , shader_stage(shader_stage)
        , type(type)
    {
        vk::BufferUsageFlags buffer_usage;

        switch (type) {
        case vk::DescriptorType::eUniformBuffer:
            buffer_usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
            break;
        case vk::DescriptorType::eStorageBuffer:
            buffer_usage = vk::BufferUsageFlagBits::eStorageBuffer;
            break;
        default:
            break;
        }
        
        buffer = Buffer::create_buffer(this->data.data(), sizeof(T) * data.size(), buffer_usage);
        Context::Get_Singleton()->get_debugger()->set_name(buffer, "des_buffer");
    }
    // Uniform_Stuff() = default;
    vk::ShaderStageFlags shader_stage;
    vk::DescriptorType type { vk::DescriptorType::eUniformBuffer };
    std::shared_ptr<Buffer> buffer;

private:
    std::vector<T> data;
};
class UniformManager {
public:
    template <typename T>
    static std::shared_ptr<Uniform_Stuff<T>> make_uniform(std::initializer_list<T> init_list, vk::ShaderStageFlags shader_stage, vk::DescriptorType type)
    {
        return std::shared_ptr<Uniform_Stuff<T>> { new Uniform_Stuff(std::vector<T> { init_list }, shader_stage, type) };
    }
    template <typename T>
    static std::shared_ptr<Uniform_Stuff<T>> make_uniform(std::vector<T> t, vk::ShaderStageFlags shader_stage, vk::DescriptorType type)
    {
        return std::shared_ptr<Uniform_Stuff<T>> { new Uniform_Stuff(std::move(t), shader_stage, type) };
    }
};

}