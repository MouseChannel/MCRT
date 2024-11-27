#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Rendering/GraphicContext.hpp"

#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/SwapChain.hpp"
// #include "shaders/Set_binding.h"
#include <vector>

namespace MCRT {

Graphic_Pipeline::~Graphic_Pipeline()
{
    Get_Context_Singleton()->get_device()->get_handle().destroyPipelineLayout(pipelineLayout);
    Get_Context_Singleton()->get_device()->get_handle().destroyPipeline(m_handle);
}
Graphic_Pipeline::Graphic_Pipeline(std::shared_ptr<RenderPass> render_pass,
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
                                   vk::PipelineColorBlendAttachmentState blendState)
{

    m_vertexShader = std::make_shared<ShaderModule>(vertShaderstr);
    m_fragShader = std::make_shared<ShaderModule>(fragShaderstr);
    shader_stage.resize(2);
    shader_stage[0]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(m_vertexShader->get_handle());
    shader_stage[1]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(m_fragShader->get_handle());

    auto binds = Vertex::make_bind();
    auto attrs = Vertex::make_attr();
    Make_VertexInput(binds, attrs);
    Make_VertexAssembly();
    Make_viewPort();
    Make_MultiSample(sampleCount);
    Make_Resterization(cullMode);
    Make_Subpass_index(subpassIndex);
    Make_DepthTest(depthTest, depthWrite);

    
    for (int i = 0; i < attachCount; i++) {

        attachs.push_back(blendState);
    }
   

    Make_Blend();
    Make_Layout(descriptorSets, pc_size, push_stage);
    
    Build_Pipeline(render_pass);
}

Graphic_Pipeline::Graphic_Pipeline(std::shared_ptr<RenderPass> render_pass,
                                   std::string vertShaderstr,
                                   std::string fragShaderstr,
                                   vk::CullModeFlagBits cullMode,
                                   vk::PipelineDepthStencilStateCreateInfo depth_stencil_state,
                                   vk::SampleCountFlagBits sampleCount,
                                   int subpassIndex,
                                   std::initializer_list<std::shared_ptr<DescriptorSet>> descriptorSets,
                                   int pc_size,
                                   vk::ShaderStageFlags push_stage,
                                   int attachCount,
                                   vk::PipelineColorBlendAttachmentState blendState)
{

    m_vertexShader = std::make_shared<ShaderModule>(vertShaderstr);
    m_fragShader = std::make_shared<ShaderModule>(fragShaderstr);
    shader_stage.resize(2);
    shader_stage[0]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(m_vertexShader->get_handle());
    shader_stage[1]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(m_fragShader->get_handle());

    auto binds = Vertex::make_bind();
    auto attrs = Vertex::make_attr();
    Make_VertexInput(binds, attrs);
    Make_VertexAssembly();
    Make_viewPort();
    Make_MultiSample(sampleCount);
    Make_Resterization(cullMode);
    Make_Subpass_index(subpassIndex);
    depth_test = depth_stencil_state;
    // Make_DepthTest(depthTest, depthWrite);


    for (int i = 0; i < attachCount; i++) {

        attachs.push_back(blendState);
    }


    Make_Blend();
    Make_Layout(descriptorSets, pc_size, push_stage);

    Build_Pipeline(render_pass);
}
 
void Graphic_Pipeline::Build_Pipeline(std::shared_ptr<RenderPass> render_pass)
{

    std::vector<vk::DynamicState> dynamic_state_value = { vk::DynamicState::eViewport,
                                                          vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.setDynamicStates(dynamic_state_value);
    vk::GraphicsPipelineCreateInfo create_info;
    create_info.setLayout(pipelineLayout)
        .setRenderPass(render_pass->get_handle())
        // vertex input
        .setPVertexInputState(&input_state)
        // vertex assembly
        .setPInputAssemblyState(&input_assembly)
        // viewport and scissor
        .setPViewportState(&viewportInfo)
        // shader
        .setStages(shader_stage)
        .setSubpass(subpass_index)
        // rasterization
        .setPRasterizationState(&rasterization_info)
        .setPMultisampleState(&multi_sample)
        .setPDepthStencilState(&depth_test)
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
        // .setPDynamicState(
        //     &vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamic_state))

        .setPDynamicState(
            &dynamic_state)

#endif
        .setPColorBlendState(&blend);

    auto [res, value] = Get_Context_Singleton()->get_device()->get_handle().createGraphicsPipeline(
        nullptr,
        create_info);

    if (res != vk::Result::eSuccess)
        throw std::runtime_error("fail to create graphic pipeline ");
    m_handle = value;
}

void Graphic_Pipeline::Make_Subpass_index(int _subpass_index)
{
    subpass_index = _subpass_index;
}

void Graphic_Pipeline::Make_ShaderStage(std::shared_ptr<ShaderModule> vertShader, std::shared_ptr<ShaderModule> fragShader)
{
    shader_stage.resize(2);
    shader_stage[0]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vertShader->get_handle());
    shader_stage[1]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fragShader->get_handle());
}
void Graphic_Pipeline::Make_VertexInput(
    vk::VertexInputBindingDescription bind,
    std::vector<vk::VertexInputAttributeDescription> attr)
{
    m_binds = bind;
    m_attrs = attr;
    input_state.setVertexAttributeDescriptions(m_attrs)
        .setVertexBindingDescriptions(m_binds);
}

void Graphic_Pipeline::Make_VertexAssembly()
{
    input_assembly.setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(false);
}

void Graphic_Pipeline::Make_viewPort()
{
    auto extent2D = Get_Context_Singleton()
                        ->get_swapchain()
                        ->Get_Extent2D();
    viewport.setX(0)
        .setY(0)
        .setHeight(extent2D.height)
        .setWidth(extent2D.width)
        .setMinDepth(0)
        .setMaxDepth(1);

    scissor.setExtent(extent2D)
        .setOffset(vk::Offset2D { 0, 0 });
    viewportInfo.setViewports(viewport)
        .setScissors(scissor)
        .setViewportCount(1)
        .setScissorCount(1);
}

void Graphic_Pipeline::Add_Shader_Modules(vk::ShaderModule module,
                                          vk::ShaderStageFlagBits stage)
{
    vk::PipelineShaderStageCreateInfo shader_create_info;
    shader_create_info.setModule(module)
        .setPName("main")
        .setStage(stage);
    shader_stage.push_back(shader_create_info);
}

void Graphic_Pipeline::Make_Resterization(vk::CullModeFlags cull_mode)
{
    rasterization_info.setCullMode(cull_mode)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        // .setFrontFace(vk::FrontFace::eClockwise)
        .setLineWidth(1)
        .setPolygonMode(vk::PolygonMode::eFill);
    //        .setRasterizerDiscardEnable(false);
}

void Graphic_Pipeline::Make_MultiSample(vk::SampleCountFlagBits sample_count)
{
    multi_sample
        .setSampleShadingEnable(sample_count != vk::SampleCountFlagBits::e1)
        .setRasterizationSamples(sample_count);
}

void Graphic_Pipeline::Make_DepthTest(bool enable_test, bool enable_write)
{
    depth_test.setDepthTestEnable(enable_test)
        .setDepthWriteEnable(enable_write);
    vk::StencilOpState stencil_op_state;

    if (enable_test)
        depth_test.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
}

void Graphic_Pipeline::Make_Blend()
{
    blend.setAttachments(attachs).setLogicOpEnable(false);
}
void Graphic_Pipeline::Make_OpacityAttach(int attach_count)
{
    vk::PipelineColorBlendAttachmentState attach;
    attach.setBlendEnable(false)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eA |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR)
        .setSrcColorBlendFactor(vk::BlendFactor::eZero)
        .setDstColorBlendFactor(vk::BlendFactor::eZero)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eZero)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);
    for (int i = 0; i < attach_count; i++) {
        attachs.push_back(attach);
    }
}
void Graphic_Pipeline::Make_AlphaAttach(int attach_count)
{
    vk::PipelineColorBlendAttachmentState attach;
    attach.setBlendEnable(true)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eA |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eR)
        .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);

    for (int i = 0; i < attach_count; i++) {

        attachs.push_back(attach);
    }
}

}