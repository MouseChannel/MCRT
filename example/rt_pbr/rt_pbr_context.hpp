#pragma once
#include "Rendering/Context.hpp"
#include "example/base/raytracing_context.hpp"
#include "example/rt_pbr/shader/Push_Constants.h"
#include "shaders/PBR/IBL/push_constants.h"
#include <iostream>

namespace MCRT {
class Buffer;
class Skybox;
class Texture;
class rt_pbr_context : public ray_tracing_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
                         //  Compute_LUT,
                         Compute };
    rt_pbr_context();
    ~rt_pbr_context();

    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;

    static float light_pos_x, light_pos_y, light_pos_z, roughness, met;
    static int apply_normal;
    static bool use_normal_map,use_abedo,use_RM_map;
    void prepare(std::shared_ptr<Window> window) override;
    // vk::Extent2D last_extent2d;
    void re_create_context() override;

private:
    // void pre_compute();
    std::shared_ptr<CommandBuffer> BeginComputeFrame() override;
    std::shared_ptr<CommandBuffer> BeginRTFrame() override;

    void EndComputeFrame() override;

    void EndRTFrame() override;

    PushContant_rtpbr pushContant_Ray;
    PushContant_IBL pushContant_IBL;
    std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
    std::shared_ptr<Skybox> m_skybox;
    std::shared_ptr<Mesh> skybox_mesh;
    // std::shared_ptr<Image> LUT;
    // std::shared_ptr<Skybox> irradiance;
    std::shared_ptr<Texture> test_texture;
    std::shared_ptr<Image> test_image;
};

}