#pragma once

#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context_base.hpp"
#include "shader/Data_struct.h"
namespace MCRT {
struct test_Compute {
    int a, b;
};
class CommandBuffer;
class Compute_Pipeline;
class Compute_Context : public Context_base {
public:
    std::shared_ptr<Image> get_out_image();
    void prepare() override;
    void post_prepare() override;
    std::shared_ptr<Pipeline_base> get_pipeline() override;

    std::shared_ptr<CommandBuffer> get_commandbuffer() override;

    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit() override;
    void EndFrame() override;

    void record_command(std::shared_ptr<CommandBuffer>) override;
    void prepare_descriptorset(std::function<void()> prepare_func) override;

    void set_barrier(std::shared_ptr<CommandBuffer> cmd);
    void re_create() override;

    void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules, std::vector<std::shared_ptr<DescriptorSet>> sets, int push_constants_size) override;

private:
    // void Make_DescriptorSet();

    std::shared_ptr<CommandBuffer> m_command_buffer;
    std::shared_ptr<Compute_Pipeline> m_compute_pipeline;
    std::shared_ptr<Image> m_out_image;
};
}