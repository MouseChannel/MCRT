#pragma once
#include "Rendering/Context.hpp"
#include "shader/Data_struct.h"
#include "example/base/raytracing_context.hpp"
#include <iostream>
namespace MCRT {
class Buffer;
class mirror_context : public ray_tracing_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
    };
    mirror_context();
    ~mirror_context();
   std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;
    static float light_pos_x, light_pos_y, light_pos_z;
    void prepare(std::shared_ptr<Window> window) override;

private:
    // std::shared_ptr<CommandBuffer> BeginGraphicFrame();
    std::shared_ptr<CommandBuffer> BeginComputeFrame() override;
    std::shared_ptr<CommandBuffer> BeginRTFrame() override;

    void EndComputeFrame() override;
    // void EndGraphicFrame();
    void EndRTFrame() override;
    PushContant pushContant_Ray;
    std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
};

}