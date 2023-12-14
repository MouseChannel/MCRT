#include "Rendering/Noise/NoiseManager.hpp"
#include "Wrapper/Image.hpp"
#include "Rendering/ComputePass.hpp"
#include "Wrapper/Shader_module.hpp"
#include "shaders/Noise/binding.h"
#include "Helper/CommandManager.hpp"
#include <Wrapper/Pipeline/Pipeline_base.hpp>
#include <numeric>
#include <random>

namespace MCRT {
    void NoiseManager::InitPerlinNoise() {
        make_permutations();

        perlin_noise.reset(new Image(128,
                                     128,
                                     128,
                                     vk::Format::eR32G32B32A32Sfloat,
                                     vk::ImageType::e3D,
                                     vk::ImageTiling::eOptimal,
                                     vk::ImageUsageFlagBits::eStorage|vk::ImageUsageFlagBits::eSampled,
                                     vk::ImageAspectFlagBits::eColor,
                                     vk::SampleCountFlagBits::e1));
        perlin_noise->SetImageLayout(vk::ImageLayout::eGeneral,
                                     vk::AccessFlagBits::eNone,
                                     vk::AccessFlagBits::eNone,
                                     vk::PipelineStageFlagBits::eTopOfPipe,
                                     vk::PipelineStageFlagBits::eBottomOfPipe);
        context.reset(new ComputePass);
        context->set_constants_size(sizeof(PushContant_Noise));
        context->prepare();
        context->prepare_descriptorset([&]() {
            auto descriptor_manager = context->get_descriptor_manager();
            descriptor_manager->Make_DescriptorSet(permutations,
                                                   (int) NoiseBinding::e_permutations,
                                                   DescriptorManager::Compute);
            descriptor_manager->Make_DescriptorSet(std::vector{perlin_noise},
                                                   DescriptorManager::Compute,
                                                   (int) NoiseBinding::e_noiseData,
                                                   vk::DescriptorType::eStorageImage,
                                                   vk::ShaderStageFlagBits::eCompute);
        });

    }

    void NoiseManager::pre_compute_PerlinNoise() {
        std::shared_ptr<ShaderModule>
                compute_shader{
                new ShaderModule("/home/mocheng/project/MCRT/shaders/Noise/PerlinNoise.comp.spv")
        };
        context->prepare_pipeline({compute_shader},
                                  {context->get_descriptor_manager()
                                           ->get_DescriptorSet(DescriptorManager::Compute)},
                                  sizeof(PushContant_Noise));
        context->post_prepare();
        // doing the actually work

        CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                                   [&](vk::CommandBuffer &cmd) {
                                       cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                              context->get_pipeline()->get_layout(),
                                                              0,
                                                              context->get_pipeline()->get_descriptor_sets(),
                                                              {});
                                       cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
                                                        context->get_pipeline()->get_handle());

                                       cmd.dispatch(noise_size, noise_size, noise_size);
                                   });
        std::cout<<"here"<<std::endl;
    }

    void NoiseManager::make_permutations() {
        std::vector<int> pre_permutations(512);
        std::vector<uint8_t> plookup;
        plookup.resize(256);
        std::iota(plookup.begin(), plookup.end(), 0);
        std::default_random_engine rndEngine(std::random_device{}());
        std::shuffle(plookup.begin(), plookup.end(), rndEngine);

        for (uint32_t i = 0; i < 256; i++) {
            pre_permutations[i] = pre_permutations[256 + i] = plookup[i];
        }

        permutations = UniformManager::make_uniform(pre_permutations, vk::ShaderStageFlagBits::eCompute|vk::ShaderStageFlagBits::eMissKHR,
                                                    vk::DescriptorType::eStorageBuffer);
    }

}
