#pragma once

// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
// #include "Wrapper/RenderPass.hpp"
#include "Wrapper/Pipeline/Pipeline_base.hpp"

namespace vk {
struct OpacityBlendAttachmentState : PipelineColorBlendAttachmentState {

    OpacityBlendAttachmentState()
    {
        setBlendEnable(false)
            .setColorWriteMask(
                vk::ColorComponentFlagBits::eA |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcColorBlendFactor(vk::BlendFactor::eZero)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eZero)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)

            .setAlphaBlendOp(vk::BlendOp::eAdd);
    }
};
}
namespace MCRT {
class ShaderModule;
class DescriptorSet;

class Graphic_Pipeline : public Pipeline_base {
public:
    Graphic_Pipeline() = default;
    ~Graphic_Pipeline();
    Graphic_Pipeline(std::shared_ptr<RenderPass> render_pass,
                     std::string vertShaderstr,
                     std::string fragShaderstr,
                     vk::CullModeFlagBits cullMode,
                     bool depthTest,
                     bool depthWrite,
                     vk::SampleCountFlagBits sampleCount,
                     int subpassIndex,
                     std::initializer_list<std::shared_ptr<DescriptorSet>> descriptorSets,
                     int pc_size,
                     vk::ShaderStageFlags push_stage,
                     int attachCount,
                     vk::PipelineColorBlendAttachmentState blendState

    );

    void Make_ShaderStage(std::shared_ptr<ShaderModule> vertShader, std::shared_ptr<ShaderModule> fragShader);
    void Make_VertexInput(vk::VertexInputBindingDescription bind,
                          std::vector<vk::VertexInputAttributeDescription> attr);
    void Make_VertexAssembly();
    void Make_viewPort();
    void Add_Shader_Modules(vk::ShaderModule shader_module, vk::ShaderStageFlagBits stage);
    void Make_Resterization(vk::CullModeFlags cull_mode = vk::CullModeFlagBits::eBack);
    void Make_DepthTest(bool enable_test = true, bool enable_write = true);
    void Make_MultiSample(vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1);
    void Make_Blend();

    void Make_OpacityAttach(int attach_count);
    void Make_Subpass_index(int subpass_index);
    void Make_AlphaAttach(int attach_count);

    void Build_Pipeline(std::shared_ptr<RenderPass> render_pass);

private:
    vk::PipelineVertexInputStateCreateInfo input_state;
    vk::PipelineInputAssemblyStateCreateInfo input_assembly;

    vk::PipelineViewportStateCreateInfo viewportInfo;

    std::vector<vk::PipelineShaderStageCreateInfo> shader_stage;
    vk::PipelineRasterizationStateCreateInfo rasterization_info;
    vk::PipelineMultisampleStateCreateInfo multi_sample;
    vk::PipelineDepthStencilStateCreateInfo depth_test;
    vk::PipelineColorBlendStateCreateInfo blend;
    std::vector<vk::PipelineColorBlendAttachmentState> attachs;
    // vk::PipelineLayout layout;

    vk::Viewport viewport;
    vk::Rect2D scissor;
    vk::VertexInputBindingDescription m_binds;
    std::vector<vk::VertexInputAttributeDescription> m_attrs;
    std::shared_ptr<ShaderModule> m_vertexShader;
    std::shared_ptr<ShaderModule> m_fragShader;

    int subpass_index;
};
} // namespace MCRT