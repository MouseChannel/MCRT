#include "Wrapper/Sampler.hpp"
#include "Wrapper/Device.hpp"
namespace MCRT {

Sampler::Sampler()
{
    vk::SamplerCreateInfo create_info;
    create_info.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eMirroredRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eMirroredRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eMirroredRepeat)
        .setAnisotropyEnable(false)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        .setCompareOp(vk::CompareOp::eNever)
        .setMaxAnisotropy(1)
        .setMipLodBias(0)
        .setAnisotropyEnable(false)
        .setMinLod(0)
        .setMaxLod(10);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .createSampler(create_info);
}
Sampler::~Sampler()
{
    Get_Context_Singleton()
        ->get_device()
        ->get_handle()
        .destroySampler(m_handle);
}
}