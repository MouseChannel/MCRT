#pragma once
#include <Helper/Instance_base.hpp>

namespace MCRT {

class Skybox;
class Image;
class ComputePass;

class IBLManager : public Instance_base<IBLManager> {
public:
    void Init(std::shared_ptr<Skybox> sky_box);
    void pre_compute_irradiance();
    void pre_compute_LUT();
    void Write_LUT_image();
    void Write_irradiance_image();

    auto get_LUT()
    {
        return LUT;
    }

    auto get_irradiance()
    {
        return irradiance;
    }

private:
    int irradiance_size = 1024;
    std::shared_ptr<Skybox> m_sky_box;
    std::shared_ptr<Image> LUT;
    std::shared_ptr<Skybox> irradiance;
    std::shared_ptr<ComputePass> context;

};


}
