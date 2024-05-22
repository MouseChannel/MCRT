#include "Rendering/PBR/IBL_Manager.hpp"
#include "Rendering/ComputeContext.hpp"
#include "Wrapper/Image.hpp"
// #include "Wrapper/Skybox.hpp"
#include "shaders/PBR/IBL/binding.h"
#include "shaders/PBR/IBL/push_constants.h"

#include "stb_image.h"

#include "Helper/DescriptorSetTarget/ImageDescriptorSetTarget.hpp"
// #include "Wrapper/ComputePass/SkyboxPass.hpp"
#include <Helper/CommandManager.hpp>
#include <Wrapper/Pipeline/Pipeline_base.hpp>
#include <Wrapper/Shader_module.hpp>
namespace MCRT {
void IBLManager::convert_skybox()
{

    m_skyboxPass.reset(new ComputePass);
    m_skyboxPass->SetShaderModule("shaders/PBR/IBL/hdr2cubemap.comp.spv");
    m_skyboxPass->Prepare_DescriptorSet([&]() {
        m_skyboxPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            m_hdr_img,
            (int)IBL_Binding::e_hdr_image,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eCombinedImageSampler,
            m_skyboxPass->get_DescriptorSet()));

        m_skyboxPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            m_skybox_img,
            (int)IBL_Binding::e_skybox,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eStorageImage,
            m_skyboxPass->get_DescriptorSet()));
    });
    m_skyboxPass->prepare_pipeline(sizeof(PushContant_IBL));
    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   m_skyboxPass->Dispach(cmd, skybox_size, skybox_size, 6);
                               });

    // context->prepare_descriptorset([&]() {
    //     auto descriptor_manager = context->get_descriptor_manager();
    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             std::vector {
    //                 ImageDescriptorData::Create(m_hdr_img->Get_Image_View(),
    //                                             m_hdr_img->Get_image_layout()) },
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_hdr_image,
    //             vk::DescriptorType::eCombinedImageSampler,
    //             vk::ShaderStageFlagBits::eCompute);
    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             std::vector {
    //                 ImageDescriptorData::Create(m_skybox_img->Get_Image_View(),
    //                                             m_skybox_img->Get_image_layout()) },
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_skybox,
    //             vk::DescriptorType::eStorageImage,
    //             vk::ShaderStageFlagBits::eCompute);
    // });
    // std::shared_ptr<ShaderModule>
    //     compute_shader {
    //         new ShaderModule("shaders/PBR/IBL/hdr2cubemap.comp.spv")
    //     };
    // context->prepare_pipeline({ compute_shader },
    //                           { context->get_descriptor_manager()
    //                                 ->get_DescriptorSet(DescriptorManager::Compute) },
    //                           sizeof(PushContant_IBL));
    // context->post_prepare();
    // CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
    //                            [&](vk::CommandBuffer& cmd) {
    //                                cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
    //                                                       context->get_pipeline()->get_layout(),
    //                                                       0,
    //                                                       context->get_pipeline()->get_descriptor_sets(),
    //                                                       {});
    //                                cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
    //                                                 context->get_pipeline()->get_handle());

    //                                cmd.dispatch(skybox_size, skybox_size, 6);
    //                            });

    m_skybox_img->generate_mipmap();
}

void IBLManager::Init(std::string path)
{

    {
        LUT.reset(new Image(LUT_SIZE,
                            LUT_SIZE,
                            // vk::Format::eR32G32B32A32Sfloat,
                            vk::Format::eR8G8B8A8Unorm,
                            vk::ImageType::e2D,
                            vk::ImageTiling::eOptimal,
                            vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
                            vk::ImageAspectFlagBits::eColor,
                            vk::SampleCountFlagBits::e1,
                            1));
        Context::Get_Singleton()->get_debugger()->set_name(LUT, "LUT");
        LUT->SetImageLayout(vk::ImageLayout::eGeneral,
                            vk::AccessFlagBits::eNone,
                            vk::AccessFlagBits::eNone,
                            vk::PipelineStageFlagBits::eTopOfPipe,
                            vk::PipelineStageFlagBits::eBottomOfPipe);
    }
    // irradiance.reset(new Skybox(irradiance_size, irradiance_size));

    auto pixel = stbi_loadf(path.c_str(), &width, &height, &channel, STBI_rgb_alpha);
    auto sizes = width * height * 4 * 4;
    {
        m_hdr_img.reset(new Image(width,
                                  height,
                                  vk::Format::eR32G32B32A32Sfloat,
                                  vk::ImageType::e2D,
                                  vk::ImageTiling::eOptimal,
                                  vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
                                  vk::ImageAspectFlagBits::eColor,
                                  vk::SampleCountFlagBits::e1,
                                  1));
        Context::Get_Singleton()->get_debugger()->set_name(m_hdr_img, "m_hdr_img");

        m_hdr_img->SetImageLayout(vk::ImageLayout::eTransferDstOptimal,
                                  vk::AccessFlagBits::eNone,
                                  vk::AccessFlagBits::eTransferWrite,
                                  vk::PipelineStageFlagBits::eTopOfPipe,
                                  vk::PipelineStageFlagBits::eTransfer);
        m_hdr_img->FillImageData(sizes, pixel);

        m_hdr_img->SetImageLayout(vk::ImageLayout::eGeneral,
                                  vk::AccessFlagBits::eTransferWrite,
                                  vk::AccessFlagBits::eTransferRead,
                                  vk::PipelineStageFlagBits::eTransfer,
                                  vk::PipelineStageFlagBits::eTransfer);
    }
    {
        m_skybox_img.reset(new Image(skybox_size,
                                     skybox_size,
                                     vk::Format::eR32G32B32A32Sfloat,
                                     vk::ImageType::e2D,
                                     vk::ImageTiling::eOptimal,
                                     vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst,

                                     vk::ImageAspectFlagBits::eColor,
                                     vk::SampleCountFlagBits::e1,
                                     6,
                                     std::log2(skybox_size),
                                     vk::ImageCreateFlagBits::eCubeCompatible));
        Context::Get_Singleton()->get_debugger()->set_name(m_skybox_img, "m_skybox_img");

        m_skybox_img->SetImageLayout(vk::ImageLayout::eGeneral,
                                     vk::AccessFlagBits::eNone,
                                     vk::AccessFlagBits::eNone,
                                     vk::PipelineStageFlagBits::eTopOfPipe,
                                     vk::PipelineStageFlagBits::eBottomOfPipe);
    }
    {
        m_irradiance_img.reset(new Image(irradiance_size,
                                         irradiance_size,
                                         vk::Format::eR16G16B16A16Sfloat,
                                         vk::ImageType::e2D,
                                         vk::ImageTiling::eOptimal,
                                         vk::ImageUsageFlagBits::eSampled |
                                             vk::ImageUsageFlagBits::eStorage |
                                             vk::ImageUsageFlagBits::eTransferSrc,
                                         vk::ImageAspectFlagBits::eColor,
                                         vk::SampleCountFlagBits::e1,
                                         6,
                                         1,
                                         vk::ImageCreateFlagBits::eCubeCompatible));
        m_irradiance_img->SetImageLayout(
            vk::ImageLayout::eGeneral,
            vk::AccessFlagBits::eTransferWrite,
            vk::AccessFlagBits::eShaderRead,
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader |
                vk::PipelineStageFlagBits::eComputeShader);
    }

    // context.reset(new ComputeContext);
    // context->set_constants_size(sizeof(PushContant_IBL));

    // context->prepare();

    convert_skybox();

    filter_skybox_mipmap();

    // context->prepare_descriptorset([&]() {
    //     auto descriptor_manager = context->get_descriptor_manager();

    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             std::vector {
    //                 ImageDescriptorData::Create(m_irradiance_img->Get_Image_View(),
    //                                             m_irradiance_img->Get_image_layout()) },
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_irradiance_image,
    //             vk::DescriptorType::eStorageImage,
    //             vk::ShaderStageFlagBits::eCompute);
    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             std::vector {
    //                 ImageDescriptorData::Create(LUT->Get_Image_View(),
    //                                             LUT->Get_image_layout()) },
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_LUT_image,
    //             vk::DescriptorType::eStorageImage,
    //             vk::ShaderStageFlagBits::eCompute);

    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             std::vector {
    //                 ImageDescriptorData::Create(m_skybox_img->Get_Image_View(),
    //                                             m_skybox_img->Get_image_layout()) },
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_skybox,
    //             vk::DescriptorType::eCombinedImageSampler,
    //             vk::ShaderStageFlagBits::eCompute);
    // });
    pre_compute_LUT();
    pre_compute_irradiance();
    Make_SkyboxMesh();
}
void IBLManager::Make_SkyboxMesh()
{
    std::vector<Vertex> vertexs {
        { { -1, 1, 1 } },
        { { -1, -1, 1 } },
        { { -1, -1, -1 } },
        { { -1, 1, 1 } },
        { { -1, -1, -1 } },
        { { -1, 1, -1 } },
        { { -1, 1, -1 } },
        { { -1, -1, -1 } },
        { { 1, -1, -1 } },
        { { -1, 1, -1 } },
        { { 1, -1, -1 } },
        { { 1, 1, -1 } },
        { { 1, 1, -1 } },
        { { 1, -1, -1 } },
        { { 1, -1, 1 } },
        { { 1, 1, -1 } },
        { { 1, -1, 1 } },
        { { 1, 1, 1 } },
        { { 1, 1, 1 } },
        { { 1, -1, 1 } },
        { { -1, -1, 1 } },
        { { 1, 1, 1 } },
        { { -1, -1, 1 } },
        { { -1, 1, 1 } },

        { { -1, 1, -1 } },
        { { 1, 1, -1 } },
        { { 1, 1, 1 } },
        { { -1, 1, -1 } },
        { { 1, 1, 1 } },
        { { -1, 1, 1 } },
        { { 1, -1, -1 } },
        { { -1, -1, -1 } },
        { { -1, -1, 1 } },
        { { 1, -1, -1 } },
        { { -1, -1, 1 } },
        { { 1, -1, 1 } }

    };

    std::vector<uint32_t> faces(vertexs.size());
    for (int i = 0; i < vertexs.size(); i++) {
        faces[i] = i;
    }
    m_skybox_mesh.reset(new Mesh("SKYBOX", vertexs, faces, {}));
}
void IBLManager::filter_skybox_mipmap()
{
    m_skyboxFilterPass.reset(new ComputePass);
    m_skyboxFilterPass->SetShaderModule("shaders/PBR/IBL/skybox_mipmap.comp.spv");
    // std::vector<std::shared_ptr<ImageDescriptorData>> skybox_mipmap_views;
    // for (int i = 1; i < std::log2(skybox_size); i++) {
    //     skybox_mipmap_views.push_back(ImageDescriptorData::Create(m_skybox_img->Get_Image_View(i), m_skybox_img->Get_image_layout()));
    // }

    std::vector<vk::ImageView> img_views;
    std::vector<vk::ImageLayout> img_layouts;
    for (int i = 1; i < std::log2(skybox_size); i++) {
        // skybox_mipmap_views.push_back(ImageDescriptorData::Create(m_skybox_img->Get_Image_View(i), m_skybox_img->Get_image_layout()));
        img_views.push_back(m_skybox_img->Get_Image_View(i));
        img_layouts.push_back(m_skybox_img->Get_image_layout());
    }
    m_skyboxFilterPass->Prepare_DescriptorSet([&]() {
        m_skyboxFilterPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            m_skybox_img,
            (int)IBL_Binding::e_skybox,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eCombinedImageSampler,
            m_skyboxFilterPass->get_DescriptorSet()));
        m_skyboxFilterPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            img_views,
            img_layouts,
            (int)IBL_Binding::e_skybox_mipmap,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eStorageImage,
            m_skyboxFilterPass->get_DescriptorSet()));
    });
    m_skyboxFilterPass->prepare_pipeline(sizeof(PushContant_IBL));
    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   for (int level = 1; level < std::log2(skybox_size); level++) {
                                       cmd.pushConstants<PushContant_IBL>(m_skyboxFilterPass->get_pipeline()->get_layout(),
                                                                          vk::ShaderStageFlagBits::eCompute,
                                                                          0,
                                                                          PushContant_IBL {
                                                                              .level = level - 1,
                                                                              .roughness = level * 1.0f / std::max(float(10), 1.0f) });
                                       m_skyboxFilterPass->Dispach(cmd, std::max(32, (skybox_size >> level) / 32), std::max(32, (skybox_size >> level) / 32), 6);
                                   }
                               });

    // context->prepare_descriptorset([&]() {
    //     auto descriptor_manager = context->get_descriptor_manager();
    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             std::vector { ImageDescriptorData::Create(m_skybox_img->Get_Image_View(), m_skybox_img->Get_image_layout()) },
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_skybox,
    //             vk::DescriptorType::eCombinedImageSampler,
    //             vk::ShaderStageFlagBits::eCompute);
    //     descriptor_manager
    //         ->Make_DescriptorSet(
    //             skybox_mipmap_views,
    //             DescriptorManager::Compute,
    //             IBL_Binding::e_skybox_mipmap,
    //             vk::DescriptorType::eStorageImage,
    //             vk::ShaderStageFlagBits::eCompute);
    // });
    // std::shared_ptr<ShaderModule>
    //     compute_shader {
    //         new ShaderModule("shaders/PBR/IBL/skybox_mipmap.comp.spv")
    //     };
    // context->prepare_pipeline({ compute_shader },
    //                           { context->get_descriptor_manager()
    //                                 ->get_DescriptorSet(DescriptorManager::Compute) },
    //                           sizeof(PushContant_IBL));
    // context->post_prepare();
    // CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
    //                            [&](vk::CommandBuffer& cmd) {
    //                                cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
    //                                                       context->get_pipeline()->get_layout(),
    //                                                       0,
    //                                                       context->get_pipeline()->get_descriptor_sets(),
    //                                                       {});
    //                                cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
    //                                                 context->get_pipeline()->get_handle());
    //                                for (int level = 1; level < std::log2(skybox_size); level++) {
    //                                    cmd.pushConstants<PushContant_SKYBOX>(context->get_pipeline()->get_layout(),
    //                                                                          vk::ShaderStageFlagBits::eCompute,
    //                                                                          0,
    //                                                                          PushContant_SKYBOX {
    //                                                                              .level = level - 1,
    //                                                                              .roughness = level * 1.0f / std::max(float(10), 1.0f) });

    //                                    cmd.dispatch(std::max(32, (skybox_size >> level) / 32),
    //                                                 std::max(32, (skybox_size >> level) / 32),
    //                                                 6);
    //                                }
    //                            });
}

void IBLManager::pre_compute_LUT()
{
    m_LUTPass.reset(new ComputePass);
    std::shared_ptr<ShaderModule>
        compute_shader {
            // new ShaderModule("shaders/PBR/IBL/lookup_table.comp.spv")
            new ShaderModule("shaders/PBR/IBL/LUT.comp.spv")
        };
    compute_shader->Set_SpecializationInfo(vk::SpecializationInfo()
                                               .setData<int>(LUT_samplecount)
                                               .setMapEntries(vk::SpecializationMapEntry()
                                                                  .setConstantID(IBL_SPEC_Binding::e_numsample_count)
                                                                  .setSize(sizeof(LUT_samplecount))));
    m_LUTPass->SetShaderModule(compute_shader);
    m_LUTPass->Prepare_DescriptorSet([&]() {
        m_LUTPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            LUT,
            (int)IBL_Binding::e_LUT_image,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eStorageImage,
            m_LUTPass->get_DescriptorSet()));
        m_LUTPass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            m_skybox_img,
            (int)IBL_Binding::e_skybox,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eCombinedImageSampler,
            m_LUTPass->get_DescriptorSet()));
    });
    m_LUTPass->prepare_pipeline(sizeof(PushContant_IBL));
    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   m_LUTPass->Dispach(cmd, LUT_SIZE / local_size, LUT_SIZE / local_size, 6);
                               });
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    std::shared_ptr<ShaderModule>
        compute_shader {
            new ShaderModule("shaders/IBL/lookup_table.comp.spv")
        };
#else
    // std::shared_ptr<ShaderModule>
    //     compute_shader {
    //         // new ShaderModule("shaders/PBR/IBL/lookup_table.comp.spv")
    //         new ShaderModule("shaders/PBR/IBL/LUT.comp.spv")
    //     };
    // compute_shader->Set_SpecializationInfo(vk::SpecializationInfo()
    //                                            .setData<int>(LUT_samplecount)
    //                                            .setMapEntries(vk::SpecializationMapEntry()
    //                                                               .setConstantID(IBL_SPEC_Binding::e_numsample_count)
    //                                                               .setSize(sizeof(LUT_samplecount))));

#endif
    // context->prepare_pipeline({ compute_shader },
    //                           { context->get_descriptor_manager()
    //                                 ->get_DescriptorSet(DescriptorManager::Compute) },
    //                           sizeof(PushContant_IBL));

    // context->post_prepare();
    // doing the actually work

    // CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
    //                            [&](vk::CommandBuffer& cmd) {
    //                                cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
    //                                                       context->get_pipeline()->get_layout(),
    //                                                       0,
    //                                                       context->get_pipeline()->get_descriptor_sets(),
    //                                                       {});
    //                                cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
    //                                                 context->get_pipeline()->get_handle());

    //                                cmd.dispatch(LUT_SIZE / local_size, LUT_SIZE / local_size, 6);
    //                            });
}

void IBLManager::pre_compute_irradiance()
{

    m_irradiancePass.reset(new ComputePass);
    std::shared_ptr<ShaderModule>
        compute_shader {
            new ShaderModule("shaders/PBR/IBL/skybox_irradiance.comp.spv")
        };
    compute_shader->Set_SpecializationInfo(vk::SpecializationInfo()
                                               .setData<int>(irradiance_samplecount)
                                               .setMapEntries(vk::SpecializationMapEntry()
                                                                  .setConstantID(IBL_SPEC_Binding::e_numsample_count)
                                                                  .setSize(sizeof(irradiance_samplecount))));
    m_irradiancePass->SetShaderModule(compute_shader);
    m_irradiancePass->Prepare_DescriptorSet([&]() {
        m_irradiancePass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            m_skybox_img,
            (int)IBL_Binding::e_skybox,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eCombinedImageSampler,
            m_irradiancePass->get_DescriptorSet()));
        m_irradiancePass->AddDescriptorSetTarget(std::make_shared<ImageDescriptorSetTarget>(
            m_irradiance_img,
            (int)IBL_Binding::e_irradiance_image,
            vk::ShaderStageFlagBits::eCompute,
            vk::DescriptorType::eStorageImage,
            m_irradiancePass->get_DescriptorSet()));
    });
    m_irradiancePass->prepare_pipeline(sizeof(PushContant_IBL));
    CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
                               [&](vk::CommandBuffer& cmd) {
                                   m_irradiancePass->Dispach(cmd,
                                                             irradiance_size / local_size,
                                                             irradiance_size / local_size,
                                                             6);
                               });

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    std::shared_ptr<ShaderModule>
        compute_shader {
            new ShaderModule("shaders/IBL/irradiance.comp.spv")
        };
#else
    // std::shared_ptr<ShaderModule>
    //     compute_shader {
    //         new ShaderModule("shaders/PBR/IBL/skybox_irradiance.comp.spv")
    //     };
    // compute_shader->Set_SpecializationInfo(vk::SpecializationInfo()
    //                                            .setData<int>(irradiance_samplecount)
    //                                            .setMapEntries(vk::SpecializationMapEntry()
    //                                                               .setConstantID(IBL_SPEC_Binding::e_numsample_count)
    //                                                               .setSize(sizeof(irradiance_samplecount))));
#endif
    // context->prepare_pipeline({ compute_shader },
    //                           { context->get_descriptor_manager()
    //                                 ->get_DescriptorSet(DescriptorManager::Compute) },
    //                           sizeof(PushContant_IBL));

    // context->post_prepare();
    // // doing the actually work

    // CommandManager::ExecuteCmd(Context::Get_Singleton()->get_device()->Get_Graphic_queue(),
    //                            [&](vk::CommandBuffer& cmd) {
    //                                cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
    //                                                       context->get_pipeline()->get_layout(),
    //                                                       0,
    //                                                       context->get_pipeline()->get_descriptor_sets(),
    //                                                       {});
    //                                cmd.bindPipeline(vk::PipelineBindPoint::eCompute,
    //                                                 context->get_pipeline()->get_handle());

    //                                cmd.dispatch(irradiance_size / local_size, irradiance_size / local_size, 6);
    //                            });
}
void IBLManager::Release()
{
    m_skybox_mesh.reset();
    m_skybox_img.reset();
    m_hdr_img.reset();
    LUT.reset();
    m_irradiance_img.reset();
    m_skyboxPass.reset();
    m_skyboxFilterPass.reset();
    m_LUTPass.reset();
    m_irradiancePass.reset();
}
}
