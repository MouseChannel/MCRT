#include "Rendering/AntiAliasing/TAA/TAA_Manager.hpp"
#include "Helper/CommandManager.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Rendering/ComputePass.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shaders/AntiAliasing/TAA/binding_taa.h"
#include "shaders/AntiAliasing/TAA/push_contants_TAA.h"

namespace MCRT {
void MCRT::TAA_Manager::Init(std::shared_ptr<Image> offscreen_image)
{
    auto pre_mvp_data = TAA_Data {
        
    };
    taa_data = UniformManager::make_uniform(
        { pre_mvp_data },
        vk::ShaderStageFlagBits::eCompute,
        vk::DescriptorType ::eStorageBuffer);

    context.reset(new ComputePass);
    context->set_constants_size(sizeof(PushContant_TAA));

    context->prepare();
    //    offscreen_image->SetImageLayout(vk::ImageLayout::eGeneral,
    //                                        vk::AccessFlagBits::eShaderWrite,
    //                                        vk::AccessFlagBits::eShaderRead,
    //                                        vk::PipelineStageFlagBits::eFragmentShader,
    //                                        vk::PipelineStageFlagBits::eComputeShader);
    context->prepare_descriptorset([&]() {
        context->get_descriptor_manager()->Make_DescriptorSet(
            taa_data,
            (int)TAA_Binding::e_taa_data,
            DescriptorManager::Compute);
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_offscreen_image,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_pre_image,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_out_image,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);

        //        std::vector<std::shared_ptr<Image>> gbuffer((int)(Gbuffer_Binding::gbuffer_count));
        //        gbuffer[(int)Gbuffer_Binding::gbuffer_position] = offscreen_image;
        //        gbuffer[(int)Gbuffer_Binding::gbuffer_last_depth] = offscreen_image;
        //        gbuffer[(int)Gbuffer_Binding::gbuffer_color] = offscreen_image;
        //        gbuffer[(int)Gbuffer_Binding::gbuffer_normal] = offscreen_image;
        //
        //        context->get_descriptor_manager()->Make_DescriptorSet(
        //            std::vector { gbuffer },
        //            DescriptorManager::Compute,
        //            (int)TAA_Binding::e_gbuffer,
        //            vk::DescriptorType::eStorageImage,
        //            vk::ShaderStageFlagBits::eCompute);

        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_last_depth,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_gbuffer_position,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
    });
    std::shared_ptr<ShaderModule>
        compute_shader {
            new ShaderModule("/home/mocheng/project/MCRT/shaders/AntiAliasing/TAA/TAA_Pass.comp.spv")
        };
    context->prepare_pipeline({ compute_shader },
                              { context->get_descriptor_manager()
                                    ->get_DescriptorSet(DescriptorManager::Compute) },
                              sizeof(PushContant_TAA));
    context->post_prepare();
}

void MCRT::TAA_Manager::TAA_Pass(std::shared_ptr<Image> cur_offscreen_image,
                                 std::shared_ptr<Image> depth_image,
                                 std::shared_ptr<Image> gbuffer_position)
{
    //    std::vector<std::shared_ptr<Image>> gbuffer((int)Gbuffer_Binding::gbuffer_count);
    //    gbuffer[(int)Gbuffer_Binding::gbuffer_color] = cur_offscreen_image;
    //    gbuffer[(int)Gbuffer_Binding::gbuffer_position] = gbuffer_position;
    context->prepare_descriptorset([&]() {
        //        context->get_descriptor_manager()->Make_DescriptorSet(
        //            std::vector { gbuffer },
        //            DescriptorManager::Compute,
        //            (int)TAA_Binding::e_gbuffer,
        //            vk::DescriptorType::eStorageImage,
        //            vk::ShaderStageFlagBits::eCompute);

        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { cur_offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_offscreen_image,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { gbuffer_position },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_gbuffer_position,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
        //        context
        context->get_descriptor_manager()->Make_DescriptorSet(
            taa_data,
            (int)TAA_Binding::e_taa_data,
            DescriptorManager::Compute);
    });

    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                          context->get_pipeline()->get_layout(),
                                                          0,
                                                          context->get_pipeline()->get_descriptor_sets(),
                                                          {});
                                   cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                                                    context->get_pipeline()->get_handle());

                                   cmd.dispatch(1200, 800, 1);
                                   //                                   cmd.copyImageToBuffer()
                               });

    //    gbuffer[(int)Gbuffer_Binding::gbuffer_color] = cur_offscreen_image;
    //    gbuffer[(int)Gbuffer_Binding::gbuffer_position] = gbuffer_position;

    context->prepare_descriptorset([&]() {
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { cur_offscreen_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_pre_image,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
   
        context->get_descriptor_manager()->Make_DescriptorSet(
            std::vector { depth_image },
            DescriptorManager::Compute,
            (int)TAA_Binding::e_last_depth,
            vk::DescriptorType::eStorageImage,
            vk::ShaderStageFlagBits::eCompute);
    });
 

    // update buffer
}

}
