#pragma once
#include "Rendering/Context.hpp"
#include <iostream>
namespace MCRT {
class Buffer;
class Blinn_phone_context : public Context {
public:
    enum Context_index { Graphic,

    };
    Blinn_phone_context();
    ~Blinn_phone_context();
    void init(std::shared_ptr<Window>) override;
    void prepare() override;
    std::shared_ptr<CommandBuffer> Begin_Frame() override;
    void EndFrame() override;
    std::shared_ptr<RT_Context> get_rt_context() override
    {

        throw std::runtime_error("no Ray_Tracing context");
    }
    std::shared_ptr<Compute_Context> get_compute_context() override
    {

        throw std::runtime_error("no compute context");
    }

    std::shared_ptr<RenderContext> get_graphic_context() override
    {
        auto base = contexts[Graphic];
        if (auto context = std::reinterpret_pointer_cast<RenderContext>(base); context != nullptr) {
            return context;
        }
        throw std::runtime_error("it is not Ray_Tracing context");
    }

private:
    std::shared_ptr<CommandBuffer> BeginGraphicFrame();

    void EndGraphicFrame();
    std::shared_ptr<Buffer> index_buffer, vertex_buffer, uv_buffer;
};

}