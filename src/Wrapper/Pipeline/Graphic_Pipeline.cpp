#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/SwapChain.hpp"
#include "shader/Set_binding.h"
#include <vector>

namespace MCRT {
Graphic_Pipeline::Graphic_Pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules)
{
    //
    vk::PipelineLayoutCreateInfo layout_create_info {};
    // todo descriptor_set
    std::vector<vk::DescriptorSetLayout> descriptor_layouts(Graphic_Set::graphic_count);

    descriptor_layouts[Graphic_Set::e_graphic] =
        Descriptor_Manager::Get_Singleton()->Get_DescriptorSet_layout(Descriptor_Manager::Graphic);
    vk::PushConstantRange push_contant;
    push_contant.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        .setOffset(0)
        .setSize(Context::Get_Singleton()
                     ->get_graphic_context()
                     ->get_constants_size());
    layout_create_info.setSetLayouts(descriptor_layouts)
        .setPushConstantRanges(push_contant);
    layout = Context::Get_Singleton()
                 ->get_device()
                 ->get_handle()
                 .createPipelineLayout(layout_create_info);

    shader_stage.resize(shader_stage_count);
    shader_stage[VERT]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits ::eVertex)
        .setModule(shader_modules[VERT]->get_handle());
    shader_stage[FRAG]
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits ::eFragment)
        .setModule(shader_modules[FRAG]->get_handle());
}
Graphic_Pipeline::~Graphic_Pipeline()
{
    Get_Context_Singleton()->get_device()->get_handle().destroyPipelineLayout(layout);
    Get_Context_Singleton()->get_device()->get_handle().destroyPipeline(m_handle);
}
void Graphic_Pipeline::Build_Pipeline(std::shared_ptr<RenderPass> render_pass)
{

    vk::GraphicsPipelineCreateInfo create_info;
    create_info.setLayout(layout)
        .setRenderPass(render_pass->get_handle())
        // vertex input
        .setPVertexInputState(&input_state)
        // vertex assembly
        .setPInputAssemblyState(&input_assembly)
        // viewport and scissor
        .setPViewportState(&viewportInfo)
        // shader
        .setStages(shader_stage)
        // rasterization
        .setPRasterizationState(&rasterization_info)
        .setPMultisampleState(&multi_sample)
        .setPDepthStencilState(&depth_test)
        .setPColorBlendState(&blend);
    auto res = Get_Context_Singleton()->get_device()->get_handle().createGraphicsPipeline(nullptr, create_info);
    // vk::GraphicsPipelineCreateInfo create_info;
    // create_info.setLayout(layout)
    //     .setRenderPass(render_pass->get_handle())
    //     // vertex input
    //     .setPVertexInputState(&input_state)
    //     // vertex assembly
    //     .setPInputAssemblyState(&input_assembly)
    //     // viewport and scissor
    //     .setPViewportState(&viewportInfo)
    //     // shader
    //     .setStages(shader_stage)
    //     // rasterization
    //     .setPRasterizationState(&rasterization_info)
    //     .setPMultisampleState(&multi_sample)
    //     // .setPDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo *pDepthStencilState_)
    //     .setPDepthStencilState(&depth_test)
    //     .setPColorBlendState(&blend);
    // auto res = Get_Context_Singleton()
    //                ->get_device()
    //                ->get_handle()
    //                .createGraphicsPipeline(nullptr, create_info);
    if (res.result != vk::Result::eSuccess)
        throw std::runtime_error("fail to create graphic pipeline ");
    m_handle = res.value;
}
void Graphic_Pipeline::Make_Layout(vk::DescriptorSetLayout descriptor_layout,
                                   uint32_t push_constants_size,
                                   vk::ShaderStageFlags push_constants_stage)
{
    vk::PushConstantRange range;
    range.setOffset(0).setSize(push_constants_size).setStageFlags(push_constants_stage);
    vk::PipelineLayoutCreateInfo createInfo;
    createInfo.setSetLayouts(descriptor_layout).setPushConstantRanges(range);
    layout = Get_Context_Singleton()
                 ->get_device()
                 ->get_handle()
                 .createPipelineLayout(createInfo);
    // layout = pipeline_layout;
}
void Graphic_Pipeline::Make_VertexInput(vk::ArrayProxyNoTemporaries<const vk::VertexInputBindingDescription> const bind,
                                        vk::ArrayProxyNoTemporaries<const vk::VertexInputAttributeDescription> const attr)
{
    input_state.setVertexAttributeDescriptions(attr).setVertexBindingDescriptions(bind);
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
        .setHeight(749)
        .setWidth(800)
        .setMinDepth(0)
        .setMaxDepth(1);

    scissor.setExtent(extent2D)
        .setOffset(vk::Offset2D { 0, 0 });
    viewportInfo.setViewports(viewport)
        .setScissors(scissor)
        .setViewportCount(1)
        .setScissorCount(1);
}
void Graphic_Pipeline::Add_Shader_Modules(vk::ShaderModule module, vk::ShaderStageFlagBits stage)
{
    vk::PipelineShaderStageCreateInfo shader_create_info;
    shader_create_info.setModule(module)
        .setPName("main")
        .setStage(stage);
    shader_stage.push_back(shader_create_info);
}
void Graphic_Pipeline::Make_Resterization()
{
    rasterization_info.setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setLineWidth(1)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setRasterizerDiscardEnable(false);
}
void Graphic_Pipeline::Make_MultiSample()
{
    multi_sample.setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);
}
void Graphic_Pipeline::Make_DepthTest()
{
    depth_test.setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp ::eLessOrEqual);
}
void Graphic_Pipeline::Make_attach()
{
    vk::PipelineColorBlendAttachmentState attach;
    attach.setBlendEnable(true)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eA |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR)
        .setSrcColorBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);
    attachs.push_back(attach);
}
void Graphic_Pipeline::Make_Blend()
{
    blend.setAttachments(attachs).setLogicOpEnable(false);
}

} // namespace MoCheng3D