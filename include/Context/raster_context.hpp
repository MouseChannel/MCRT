#pragma once
#include "Context/Context.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Shader/Data_struct.h"
#include "Shader/Data_struct.h"
#include <functional>
#include <iostream>


namespace MCRT {
class Buffer;
class Graphic_Pipeline;
class raster_context : public Context {
public:
    // enum Pass_index { Graphic,

    //                   Compute };

    raster_context();
    virtual ~raster_context();
    virtual std::shared_ptr<CommandBuffer> Begin_Frame() override;
    virtual void EndFrame() override;
    virtual void prepare(std::shared_ptr<Window> window) override;
    // TODO make a skybox context
    //    void SkyboxPass(std::shared_ptr<CommandBuffer> cmd,std::shared_ptr<GraphicContext> graphic_context, std::function<void(std::shared_ptr<CommandBuffer> cmd)> func);
    virtual void re_create_context() override;

protected:
    //    virtual std::shared_ptr<Graphic_Pipeline> get_skybox_pipeline() = 0;
    // virtual std::shared_ptr<Mesh> get_skybox_mesh() = 0;
    virtual std::shared_ptr<CommandBuffer> BeginGraphicFrame() = 0;
    virtual void EndGraphicFrame() = 0;
    std::shared_ptr<Uniform_Stuff<Camera_matrix>> camera_matrix;

private:
};
}