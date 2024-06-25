#pragma once
 
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
 
namespace MCRT {

class TransparencySubPass : public BaseSubPass {
public:
    TransparencySubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index);
    //    void set_description() override;
    ~TransparencySubPass() = default;
    //    void prepare_renderTarget()override;
    //    void run_subpass() override;
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size) override;
    void post_prepare() override;
    // void draw(vk::CommandBuffer& cmd, std::vector<std::shared_ptr<Mesh>>& meshs, void* push_constant) override;

    //      void set_subpassDependency(vk::SubpassDependency dependency) override;
private:
};
}