#pragma once
#include <functional>
#include <memory>
namespace MCRT {
class CommandBuffer;
class RenderTarget;
class Image;
class RenderPass;
class ShaderModule;
class Pipeline_base;
class Framebuffer;
class Context_base {
public:
    // [[nodiscard("missing Render_pass")]] virtual std::shared_ptr<RenderPass>& Get_render_pass() = 0;
    virtual std::shared_ptr<Pipeline_base> get_pipeline() = 0;
    virtual std::shared_ptr<CommandBuffer> get_commandbuffer() = 0;
    // virtual std::shared_ptr<Framebuffer>& get_framebuffer() = 0;
    virtual void prepare(std::vector<std::shared_ptr<ShaderModule>> shader_modules) = 0;
    virtual void post_prepare() = 0;
    virtual std::shared_ptr<CommandBuffer> BeginFrame() = 0;
    virtual void Submit() = 0;
    virtual void EndFrame() = 0;
    // std::shared_ptr<CommandBuffer> Begin_Record_Command_Buffer();
    virtual void record_command(std::shared_ptr<CommandBuffer>) = 0;

    virtual void prepare_descriptorset(std::function<void()> prepare_func) = 0;

    virtual void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules) = 0;
    void set_constants_size(int size)
    {
        push_constants_size = size;
    }
    auto get_constants_size()
    {
        return push_constants_size;
    }

protected:
    void Prepare_RenderPass();
    int push_constants_size;
};
}