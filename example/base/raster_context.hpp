#pragma once
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "shader/Data_struct.h"
#include <iostream>
namespace MCRT {
class Buffer;
class raster_context : public Context {
public:
    enum Context_index { Graphic,

                         Compute };
    raster_context();
    ~raster_context();
    virtual std::shared_ptr<CommandBuffer> Begin_Frame() override;
    virtual void EndFrame() override;
    virtual void prepare(std::shared_ptr<Window> window) override;

protected:
    virtual std::shared_ptr<CommandBuffer> BeginGraphicFrame() = 0;
    virtual void EndGraphicFrame() = 0;
    std::shared_ptr<Uniform_Stuff<Camera_matrix>> camera_matrix;

private:
};
}