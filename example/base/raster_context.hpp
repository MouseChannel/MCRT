#pragma once
#include "Helper/Uniform_Manager.hpp"
#include "Rendering/Context.hpp"
#include "example/base/shader/raster/Data_struct.h"
#include "shader/Data_struct.h"
#include <functional>
#include <iostream>

namespace MCRT {
class Buffer;
class Graphic_Pipeline;
class raster_context : public Context {
public:
    enum Context_index { Graphic,

                         Compute };
    raster_context();
    ~raster_context();
    virtual std::shared_ptr<CommandBuffer> Begin_Frame() override;
    virtual void EndFrame() override;
    virtual void prepare(std::shared_ptr<Window> window) override;
    void SkyboxPass(std::shared_ptr<CommandBuffer> cmd, std::function<void(std::shared_ptr<CommandBuffer> cmd)> func);
    virtual void re_create_context() override;

protected:
    virtual std::shared_ptr<Graphic_Pipeline> get_skybox_pipeline() = 0;
    virtual std::shared_ptr<Mesh> get_skybox_mesh() = 0;
    virtual std::shared_ptr<CommandBuffer> BeginGraphicFrame() = 0;
    virtual void EndGraphicFrame() = 0;
    std::shared_ptr<Uniform_Stuff<Camera_matrix>> camera_matrix;

private:
};
}