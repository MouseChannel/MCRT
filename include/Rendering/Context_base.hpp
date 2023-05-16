#pragma once
#include <memory>
namespace MCRT {
class CommandBuffer;
class RenderTarget;
class Image;
class RenderPass;
class Pipeline_base;
class Framebuffer;
class Context_base {
public:
    // [[nodiscard("missing Render_pass")]] virtual std::shared_ptr<RenderPass>& Get_render_pass() = 0;
    virtual std::shared_ptr<Pipeline_base> get_pipeline() = 0;
    virtual std::shared_ptr<CommandBuffer> get_commandbuffer() = 0;
    // virtual std::shared_ptr<Framebuffer>& get_framebuffer() = 0;
    virtual void prepare();
    virtual std::shared_ptr<CommandBuffer> BeginFrame() = 0;
    virtual void Submit() = 0;
    virtual void EndFrame() = 0;
    std::shared_ptr<CommandBuffer> Begin_Record_Command_Buffer();
    virtual void record_command(std::shared_ptr<CommandBuffer>) = 0;

protected:
    virtual void prepare_descriptorset() = 0;
    virtual void prepare_pipeline() = 0;

    void Prepare_RenderPass();
};
}