#include "Rendering/PBR/IBL_Manager.hpp"
#include "Rendering/ComputePass.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Skybox.hpp"
#include "shaders/PBR/IBL/binding.h"
#include "shaders/PBR/IBL/push_constants.h"

#include "Helper/DescriptorManager.hpp"

#include <Helper/CommandManager.hpp>
#include <Wrapper/Pipeline/Pipeline_base.hpp>
#include <Wrapper/Shader_module.hpp>

namespace MCRT {

void IBLManager::Init(std::shared_ptr<Skybox> skybox)
{
    m_sky_box = skybox;

    LUT.reset(new Image(1024,
                        1024,
                        // vk::Format::eR32G32B32A32Sfloat,
                        vk::Format::eR8G8B8A8Unorm,
                        vk::ImageType::e2D,
                        vk::ImageTiling::eOptimal,
                        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
                        vk::ImageAspectFlagBits::eColor,
                        vk::SampleCountFlagBits::e1));
    LUT->SetImageLayout(vk::ImageLayout::eGeneral,
                        vk::AccessFlagBits::eNone,
                        vk::AccessFlagBits::eNone,
                        vk::PipelineStageFlagBits::eTopOfPipe,
                        vk::PipelineStageFlagBits::eBottomOfPipe);
    irradiance.reset(new Skybox(irradiance_size, irradiance_size));

    context.reset(new ComputePass);
    context->set_constants_size(sizeof(PushContant_IBL));

    context->prepare();
    context->prepare_descriptorset([&]() {
        auto descriptor_manager = context->get_descriptor_manager();
        //        Descriptor_Manager::Get_Singleton()
        //            ->Make_DescriptorSet(
        //                std::vector{ irradiance->get_handle() },
        //                Descriptor_Manager::Compute,
        //                IBL_Binding::e_irradiance_image,
        //                vk::DescriptorType::eStorageImage,
        //                vk::ShaderStageFlagBits::eCompute);
        descriptor_manager
            ->Make_DescriptorSet(
                std::vector { irradiance->get_handle() },
                DescriptorManager::Compute,
                IBL_Binding::e_irradiance_image,
                vk::DescriptorType::eStorageImage,
                vk::ShaderStageFlagBits::eCompute);

        //        Descriptor_Manager::Get_Singleton()
        //            ->Make_DescriptorSet(
        //                std::vector{ LUT },
        //                Descriptor_Manager::Compute,
        //                IBL_Binding::e_LUT_image,
        //                vk::DescriptorType::eStorageImage,
        //                vk::ShaderStageFlagBits::eCompute);

        descriptor_manager
            ->Make_DescriptorSet(
                std::vector { LUT },
                DescriptorManager::Compute,
                IBL_Binding::e_LUT_image,
                vk::DescriptorType::eStorageImage,
                vk::ShaderStageFlagBits::eCompute);

        //        Descriptor_Manager::Get_Singleton()
        //            ->Make_DescriptorSet(
        //                std::vector{ m_sky_box->get_handle() },
        //                Descriptor_Manager::Compute,
        //                IBL_Binding::e_skybox,
        //                vk::DescriptorType::eCombinedImageSampler,
        //                vk::ShaderStageFlagBits::eCompute);
        descriptor_manager
            ->Make_DescriptorSet(
                std::vector { m_sky_box->get_handle() },
                DescriptorManager::Compute,
                IBL_Binding::e_skybox,
                vk::DescriptorType::eCombinedImageSampler,
                vk::ShaderStageFlagBits::eCompute);
    });
}

void IBLManager::pre_compute_LUT()
{

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    std::shared_ptr<ShaderModule>
            compute_shader {
            new ShaderModule("shaders/IBL/lookup_table.comp.spv")
    };
#else
    std::shared_ptr<ShaderModule>
        compute_shader {
            new ShaderModule("/home/mocheng/project/MCRT/shaders/PBR/IBL/lookup_table.comp.spv")
        };

#endif
    
    
    
    
    context->prepare_pipeline({ compute_shader },
                              { context->get_descriptor_manager()
                                    ->get_DescriptorSet(DescriptorManager::Compute) },
                              sizeof(PushContant_IBL));

    context->post_prepare();
    // doing the actually work

    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                          context->get_pipeline()->get_layout(),
                                                          0,
                                                          context->get_pipeline()->get_descriptor_sets(),
                                                          {});
                                   cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                                                    context->get_pipeline()->get_handle());

                                   cmd.dispatch(1024, 1024, 1);
                               });
}

void IBLManager::pre_compute_irradiance()
{
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    std::shared_ptr<ShaderModule>
            compute_shader {
            new ShaderModule("shaders/IBL/irradiance.comp.spv")
    };
#else    
    std::shared_ptr<ShaderModule>
        compute_shader {
            new ShaderModule("/home/mocheng/project/MCRT/shaders/PBR/IBL/irradiance.comp.spv")
        };
#endif
    context->prepare_pipeline({ compute_shader },
                              { context->get_descriptor_manager()
                                    ->get_DescriptorSet(DescriptorManager::Compute) },
                              sizeof(PushContant_IBL));

    context->post_prepare();
    // doing the actually work

    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                          context->get_pipeline()->get_layout(),
                                                          0,
                                                          context->get_pipeline()->get_descriptor_sets(),
                                                          {});
                                   cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                                                    context->get_pipeline()->get_handle());

                                   cmd.dispatch(irradiance_size, irradiance_size, 6);
                               });
}

}
