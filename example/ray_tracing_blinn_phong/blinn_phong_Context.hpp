#pragma once
#include "Rendering/Context.hpp"
#include "example/base/raytracing_context.hpp"
#include "shader/Data_struct.h"
#include <iostream>

namespace MCRT {
class Buffer;
class blinn_phong_context : public ray_tracing_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
                         Compute };
    blinn_phong_context();
    ~blinn_phong_context();
    // void init(std::shared_ptr<Window>) override;
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;

    static float light_pos_x, light_pos_y, light_pos_z;
    void prepare(std::shared_ptr<Window> window) override;

private:
    // std::shared_ptr<CommandBuffer> BeginGraphicFrame();
    std::shared_ptr<CommandBuffer> BeginComputeFrame()override;
    std::shared_ptr<CommandBuffer> BeginRTFrame() override;

    void EndComputeFrame()override;
    // void EndGraphicFrame();
    void EndRTFrame() override;
    // void prepare() override;
    PushContant pushContant_Ray;
    std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
};

}