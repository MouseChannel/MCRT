#pragma once
#include "Rendering/Context.hpp"
#include "shader/Data_struct.h"
#include <iostream>
namespace MCRT {
class Buffer;
class ray_tracing_context : public Context {
public:
    enum Context_index { Graphic,
                         Ray_tracing,
                         Compute };
    ray_tracing_context();
    ~ray_tracing_context();
    // void init(std::shared_ptr<Window>) override;
    virtual std::shared_ptr<CommandBuffer> Begin_Frame() override;
    virtual void EndFrame() override;
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
        auto base = contexts[Context_index::Compute];
        if (auto context = std::reinterpret_pointer_cast<Compute_Context>(base); context != nullptr) {
            return context;
        }
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