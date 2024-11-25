#pragma once

#include "Wrapper/DescriptorPool.hpp"
#include <functional>
#include <iostream>
#include <memory>

namespace MCRT {
class CommandBuffer;

class RenderTarget;

class Image;

class RenderPass;

class ShaderModule;

class DescriptorSet;

class Pipeline_base;

class Framebuffer;

class BaseContext {
public:
    virtual ~BaseContext()  = default;
    // virtual ~BaseContext() {
    //     std::cout<<"base Context"<<std::endl;
    // };

    // [[nodiscard("missing Render_pass")]] virtual std::shared_ptr<RenderPass>& Get_render_pass() = 0;
    // virtual std::shared_ptr<Pipeline_base> get_pipeline() = 0;

    virtual std::shared_ptr<CommandBuffer> get_commandbuffer() = 0;

    // virtual std::shared_ptr<Framebuffer>& get_framebuffer() = 0;
    virtual void prepare() = 0;

    virtual void post_prepare() = 0;

    virtual std::shared_ptr<CommandBuffer> BeginFrame() = 0;

    virtual void Submit() = 0;

    virtual void EndFrame() = 0;

    // std::shared_ptr<CommandBuffer> Begin_Record_Command_Buffer();
    // virtual void record_command(std::shared_ptr<CommandBuffer>) = 0;
    // need register all descriptor in the first time
    // virtual void prepare_descriptorset(std::function<void()> prepare_func) = 0;
    //    virtual void update_descriptorset(std::function<void()> update_func) = 0;

    // virtual void prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules,
    //                               std::vector<std::shared_ptr<DescriptorSet>> sets,
    //                               int push_constants_size) = 0;
    // std::shared_ptr<DescriptorManager> get_descriptor_manager()
    // {
    //     return descriptor_manager;
    // }

    // virtual void set_re_create(std::function<void()> re_create)
    // {
    //     re_create_context = re_create;
    // }
    virtual void re_create() = 0;

  
    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
    std::vector< std::shared_ptr<DescriptorPool>> descriptorSetPools;

protected:
    // std::function<void()> re_create_context = nullptr;
 

    // int push_constants_size;
    // std::shared_ptr<DescriptorManager> descriptor_manager { new DescriptorManager };
};
}