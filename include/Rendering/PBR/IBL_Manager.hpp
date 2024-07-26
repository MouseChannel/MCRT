#pragma once
#include "Rendering/Model.hpp"
#include "Wrapper/ComputePass/ComputePass.hpp"
#include <Helper/Instance_base.hpp>
namespace MCRT {

class Image;
class ComputeContext;

class IBLManager : public Instance_base<IBLManager> {
public:
    void Init(std::string path);
    void convert_skybox();
    void pre_compute_irradiance();
    void pre_compute_LUT();
    void Write_LUT_image();
    void Write_irradiance_image();
    void filter_skybox_mipmap();
    auto get_LUT()
    {
        return LUT;
    }

    auto get_irradiance()
    {
        return m_irradiance_img;
    }
    auto get_skybox()
    {
        return m_skybox_img;
    }
    auto get_skybox_mesh()
    {
        return m_skybox_mesh;
    }
    void Release();

private:
    void Make_SkyboxMesh();
    const int irradiance_size = 32;
    const int irradiance_samplecount = 4096;
    const int LUT_SIZE = 512;
    const int LUT_samplecount = 1024;

    const int skybox_size = 1024;
    int width, height, channel;

    std::shared_ptr<Mesh> m_skybox_mesh;
    std::shared_ptr<Image> m_skybox_img;
    std::shared_ptr<Image> m_hdr_img;
    // std::shared_ptr<Skybox> m_sky_box;
    std::shared_ptr<Image> LUT;
    std::shared_ptr<Image> m_irradiance_img;

    // std::shared_ptr<ComputeContext> context;

    std::shared_ptr<ComputePass<>> m_skyboxPass;
    std::shared_ptr<ComputePass<>> m_skyboxFilterPass;
    std::shared_ptr<ComputePass<>> m_LUTPass;
    std::shared_ptr<ComputePass<>> m_irradiancePass;
};

}
