#pragma once
#include "example/base/raster_context.hpp"
// #include "shaders/Data_struct.h"
#include "Rendering/Render_Context.hpp"
#include "example/raster/shader/Constants.h"
#include <iostream>
#include "Rendering/RT_Context.hpp"

namespace MCRT {
class Buffer;
class Skybox;
class raster_context_pbr : public raster_context {
public:
    enum Context_index { Graphic,
                         Sky_box,
                         Ray_tracing,
    };
    raster_context_pbr();
    ~raster_context_pbr();
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;
    std::shared_ptr<RT_Context> get_rt_context() override
    {
        auto base = contexts[Context_index::Ray_tracing];
        if (auto context = std::dynamic_pointer_cast<RT_Context>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }
    std::shared_ptr<Compute_Context> get_compute_context() override
    {
        // auto base = contexts[Context_index::Compute];
        // if (auto context = std::reinterpret_pointer_cast<Compute_Context>(base); context != nullptr) {
        //     return context;
        // }
        throw std::runtime_error("it is not compute context");
    }

    std::shared_ptr<RenderContext> get_graphic_context() override
    {
        auto base = contexts[Context_index::Graphic];
        if (auto context = std::dynamic_pointer_cast<RenderContext>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }
    static float light_pos_x, light_pos_y, light_pos_z;
    static bool use_normal_map, use_r_rm_map;

    void prepare(std::shared_ptr<Window> window) override;
    std::shared_ptr<Graphic_Pipeline> get_skybox_pipeline() override
    {
        return get_graphic_context()->get_skybox_pipeline();
    }
    std::shared_ptr<Mesh> get_skybox_mesh() override
    {
        return skybox_mesh;
    }

private:
    std::shared_ptr<CommandBuffer> BeginGraphicFrame() override;
    std::shared_ptr<CommandBuffer> BeginComputeFrame();
    std::shared_ptr<CommandBuffer> BeginSkyboxFrame();
    void EndSkyboxFrame();
    void EndComputeFrame();
    void EndGraphicFrame() override;

    // PushContant pushContant_Ray;
    PC_Raster pc;
    float angle;
    std::shared_ptr<Mesh> skybox_mesh;
    std::shared_ptr<Skybox> sky_box;
    std::shared_ptr<Image> LUT;
    std::shared_ptr<Skybox> irradiance;
};

}