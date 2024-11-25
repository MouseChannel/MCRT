#include "Wrapper/Sampler.hpp"
#include "Wrapper/Device.hpp"
namespace MoCheng3D {

Sampler::Sampler()
{
    vk::SamplerCreateInfo create_info;
    create_info.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        .setAnisotropyEnable(false)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear);
    m_handle = Get_Context_Singleton()
                   ->Get_Device()
                   ->Get_handle()
                   .createSampler(create_info);
}
Sampler::~Sampler()
{
    Get_Context_Singleton()
        ->Get_Device()
        ->Get_handle()
        .destroySampler(m_handle);
}
}