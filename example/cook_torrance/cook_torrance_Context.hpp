#pragma once
#include "Rendering/Context.hpp"
#include "example/base/raytracing_context.hpp"
#include "shader/Data_struct.h"
#include "example/cook_torrance/shader/push_constants/push_constants.h"
#include <iostream>

namespace MCRT {
class Buffer;
class cook_torrance_context : public ray_tracing_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
                         Compute };
    cook_torrance_context();
    ~cook_torrance_context();
    // void init(std::shared_ptr<Window>) override;
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;
    
    static float roughness;
    static float metallicness;

    void prepare(std::shared_ptr<Window> window) override;
private:
    // std::shared_ptr<CommandBuffer>
    // BeginGraphicFrame();
    std::shared_ptr<CommandBuffer> BeginComputeFrame() override;
    std::shared_ptr<CommandBuffer> BeginRTFrame() override;

    void EndComputeFrame() override;
    // void EndGraphicFrame();
    void EndRTFrame() override;
    PushContant_cook_torrance pushContant_Ray;
    std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
    std::shared_ptr<Buffer> m_vertex_buffer;
};

}