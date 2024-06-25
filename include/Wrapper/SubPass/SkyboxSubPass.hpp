#pragma once
// #include "Rendering/Render_Target/DepthTarget.hpp"
#include "Rendering/Render_Target/SwapChainTarget.hpp"
#include "include/Wrapper/SubPass/BaseSubPass.hpp"
#include "memory"
#include <functional>
namespace MCRT {

class SkyboxSubPass : public BaseSubPass {
public:
    SkyboxSubPass(std::weak_ptr<GraphicContext> graphicContext, int subpass_index);
    //    void set_description() override;
    ~SkyboxSubPass() = default;
    //    void prepare_renderTarget()override;
    //    void run_subpass() override;
    void prepare_vert_shader_module(std::string vert_shader) override;
    void prepare_frag_shader_module(std::string frag_shader) override;
    //    void  prepare_descriptorset(std::function<void()> prepare)override;
    void prepare_pipeline(int pc_size ) override;
    void post_prepare() override;
    // void Prepare_DescriptorSet(std::function<void()> prepare) override;
    void Make_SkyboxMesh();
    int get_DescriptorSetCount() override
    {
        return 1;
    }
    // vk::DescriptorSet get_DescriptorSet(int index = 0) override
    // {
    //     return m_descriptorSet->get_handle()[index];
    // }
    // void draw(vk::CommandBuffer& cmd, std::vector<std::shared_ptr<Mesh>>& meshs, void* push_constant) override;

    //      void set_subpassDependency(vk::SubpassDependency dependency) override;
private:
};
}