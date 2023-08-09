#pragma once
#include "Rendering/Context.hpp"
#include "example/base/raytracing_context.hpp"
#include "example/ray_tracing_AO/shader/Binding.h"
#include <iostream>

namespace MCRT {
class Buffer;
class raytracing_AO_context : public ray_tracing_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
                         //  Compute
    };
    raytracing_AO_context();
    ~raytracing_AO_context();
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;

    void prepare(std::shared_ptr<Window> window) override;
    static bool enable_ao;
    static int sample_count;
    static float ao_field;

private:
    std::shared_ptr<CommandBuffer>
    BeginComputeFrame() override;
    std::shared_ptr<CommandBuffer> BeginRTFrame() override;

    void EndComputeFrame() override;
    void EndRTFrame() override;
    AOPushContant pushContant_Ray;
    std::shared_ptr<Buffer> m_vertex_buffer;
    std::shared_ptr<Buffer> index_buffer, position_buffer, uv_buffer;
    std::shared_ptr<Image> ao_image;
};

}