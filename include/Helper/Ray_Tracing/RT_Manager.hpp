#pragma once
#include "Helper/Debugger.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"

namespace MCRT {

enum RT_Binding {
    e_tlas = 0,
    e_out_image = 1

};
static std::shared_ptr<Image> rt_out_image;
static void create_rt_descriptor_set()
{
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(AS_Builder::Get_Singleton()->get_tlas(),
                             RT_Binding::e_tlas,
                             vk::DescriptorType::eAccelerationStructureKHR,
                             vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR);
    rt_out_image.reset(new Image(800,
                                 800,
                                 vk::Format::eR32G32B32A32Sfloat,
                                 vk::ImageType::e2D,
                                 vk::ImageTiling::eOptimal,
                                 vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                                 vk::ImageAspectFlagBits::eColor,
                                 vk::SampleCountFlagBits::e1));
    Context::Get_Singleton()->get_debugger()->set_name(rt_out_image, "out_image");
    rt_out_image->SetImageLayout(vk::ImageLayout::eGeneral,
                                 vk::AccessFlagBits::eNone,
                                 vk::AccessFlagBits::eNone,
                                 vk::PipelineStageFlagBits ::eTopOfPipe,
                                 vk::PipelineStageFlagBits::eBottomOfPipe);

    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(rt_out_image,
                             RT_Binding::e_out_image,
                             vk::DescriptorType::eStorageImage,
                             vk::ShaderStageFlagBits::eRaygenKHR);
 
    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool();
    Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet();
    int d = 0;
}

}