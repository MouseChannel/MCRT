#pragma once
#include "Helper/DescriptorManager.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
namespace MCRT {

enum RT_Binding {
    e_tlas = 0,
    e_out_image = 1

};
static void create_rt_descriptor_set()
{
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(AS_Builder::Get_Singleton()->get_tlas(), RT_Binding::e_tlas, vk::DescriptorType::eAccelerationStructureKHR, vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR);
    Descriptor_Manager::Get_Singleton()
        ->Make_DescriptorSet(AS_Builder::Get_Singleton()->get_tlas(), RT_Binding::e_out_image, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eRaygenKHR);
    Descriptor_Manager::Get_Singleton()->CreateDescriptorPool();
    Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet();
}
}