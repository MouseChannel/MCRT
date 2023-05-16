#pragma once

#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context_base.hpp"
namespace MCRT {
struct test_Compute {
    int a, b;
};
class CommandBuffer;
class Compute_Pipeline;
class Compute_Context : public Context_base {
public:
    void prepare() override;
    std::shared_ptr<Pipeline_base> get_pipeline() override;

    std::shared_ptr<CommandBuffer> get_commandbuffer()override ;

    std::shared_ptr<CommandBuffer> BeginFrame() override;
    void Submit()override;
    void EndFrame() override;

    void record_command(std::shared_ptr<CommandBuffer>) override;
      void prepare_descriptorset() override;
    // virtual void prepare_pipeline() override;

private:
    void create_uniform_buffer();
    // void Make_DescriptorSet();
    void prepare_pipeline() override;
    std::shared_ptr<Uniform_Stuff<test_Compute>> test_data;
    std::shared_ptr<CommandBuffer> m_command_buffer;
    std::shared_ptr<Compute_Pipeline> m_compute_pipeline;
};
}