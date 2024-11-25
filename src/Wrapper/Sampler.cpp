#include "Wrapper/Sampler.hpp"
#include "Wrapper/Device.hpp"
 
#include <cfloat>
namespace MCRT {

Sampler::Sampler()
{
    vk::SamplerCreateInfo create_info;
    create_info.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setBorderColor(vk::BorderColor::eFloatTransparentBlack)
        .setAnisotropyEnable(true)
        .setMaxAnisotropy(Context::Get_Singleton()->get_device()->get_properties().limits.maxSamplerAnisotropy)
        .setMinLod(0)
        .setMaxLod(FLT_MAX)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        // .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        // .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        // .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        // .setUnnormalizedCoordinates(false)
        // .setCompareEnable(false)
        // .setCompareOp(vk::CompareOp::eNever)
        // .setMipLodBias(0)
         ;
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