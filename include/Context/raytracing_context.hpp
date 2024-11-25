#pragma once
#include "Context/Context.hpp"
#include "example/base/shaders/ray_tracing/Data_struct.h"
#include "shaders/Data_struct.h"
#include <iostream>

namespace MCRT {
class Buffer;
class ray_tracing_context : public Context {
public:
    enum Pass_index { Graphic,
                      Ray_tracing,
                      Compute };
    ray_tracing_context();
    ~ray_tracing_context();
    // void init(std::shared_ptr<Window>) override;
    virtual std::shared_ptr<CommandBuffer> Begin_Frame() override;
    virtual void EndFrame() override;
    std::shared_ptr<RaytracingPass> get_rt_context() override
    {
        auto base = PASS[Pass_index::Ray_tracing];
        if (auto context = std::reinterpret_pointer_cast<RaytracingPass>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }
    std::shared_ptr<ComputePass> get_compute_context() override
    {
        auto base = PASS[Pass_index::Compute];
        if (auto context = std::reinterpret_pointer_cast<ComputePass>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not compute context");
    }

    std::shared_ptr<GraphicContext> get_graphic_context() override
    {
        auto base = PASS[Pass_index::Graphic];
        if (auto context = std::reinterpret_pointer_cast<GraphicContext>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }

    virtual void prepare(std::shared_ptr<Window> window) override;
    virtual void re_create_context() override;

private:
    std::shared_ptr<CommandBuffer> BeginGraphicFrame();
    virtual std::shared_ptr<CommandBuffer> BeginComputeFrame() = 0;
    virtual std::shared_ptr<CommandBuffer> BeginRTFrame() = 0;

    virtual void EndComputeFrame() = 0;
    void EndGraphicFrame();
    virtual void EndRTFrame() = 0;
    PushContant pushContant_Ray;
    std::shared_ptr<Buffer> m_vertex_buffer;
    std::shared_ptr<Buffer> index_buffer;
};

}