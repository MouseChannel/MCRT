#pragma once
#include "Rendering/Context.hpp"
#include "example/base/raytracing_context.hpp"
// #include "shader/Data_struct.h"
#include "shader/Path_tracing_light/Push_constants.h"
#include <iostream>

namespace MCRT {
class Buffer;
class Path_tracing_light_context : public ray_tracing_context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
                         Compute
    };
    Path_tracing_light_context();
    ~Path_tracing_light_context();
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;

    void prepare(std::shared_ptr<Window> window) override;

    static bool enable_dir, enable_indir, enable_filter;
    static float light_desity;

private:
    std::shared_ptr<CommandBuffer> BeginComputeFrame() override;
    std::shared_ptr<CommandBuffer> BeginRTFrame() override;

    void EndComputeFrame() override;
    void EndRTFrame() override;
    PushContant_light pushContant_Ray;
    std::shared_ptr<Buffer> m_vertex_buffer;
    std::shared_ptr<Buffer> index_buffer, position_buffer, uv_buffer;
    std::vector<glm::vec3> light_pos;
};

}