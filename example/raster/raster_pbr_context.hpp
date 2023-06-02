#pragma once
#include "example/base/raster_context.hpp"
// #include "shader/Data_struct.h"
#include "shader/Raster/Constants.h"
#include <iostream>

namespace MCRT {
class Buffer;
class raster_context_pbr : public raster_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
    };
    raster_context_pbr();
    ~raster_context_pbr();
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;
    std::shared_ptr<RT_Context> get_rt_context() override
    {
        auto base = contexts[Context_index::Ray_tracing];
        if (auto context = std::reinterpret_pointer_cast<RT_Context>(base); context != nullptr) {
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
        if (auto context = std::reinterpret_pointer_cast<RenderContext>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }
    static float light_pos_x, light_pos_y, light_pos_z;

    void prepare(std::shared_ptr<Window> window) override;

private:
    std::shared_ptr<CommandBuffer> BeginGraphicFrame() override;
    std::shared_ptr<CommandBuffer> BeginComputeFrame();

    void EndComputeFrame();
    void EndGraphicFrame() override;

    // PushContant pushContant_Ray;
    PC_Raster pc;
    float angle;
    // std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
};

}