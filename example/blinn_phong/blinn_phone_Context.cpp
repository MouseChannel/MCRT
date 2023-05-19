
#include "example/blinn_phong/blinn_phone_Context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
#include "iostream"

namespace MCRT {
std::unique_ptr<Context> Context::_instance { new MCRT::Blinn_phone_context };
Blinn_phone_context::Blinn_phone_context()
{
}
Blinn_phone_context::~Blinn_phone_context()
{
}
void Blinn_phone_context::prepare()
{
    // auto vertex_buffer = Model::models[0]->get_vertex_buffer();

    // index_buffer = Buffer::CreateDeviceBuffer(indices.data(), indices.size() * sizeof(indices[0]), vk::BufferUsageFlagBits::eIndexBuffer);
    // vertex_buffer = Buffer::CreateDeviceBuffer(positions.data(), positions.size() * sizeof(positions[0]), vk::BufferUsageFlagBits::eVertexBuffer);
    // uv_buffer = Buffer::CreateDeviceBuffer(uvs.data(), uvs.size() * sizeof(uvs[0]), vk::BufferUsageFlagBits::eVertexBuffer);
}
void Blinn_phone_context::init(std::shared_ptr<Window> window)
{
    Context::init(window);
    Obj_loader::load_model("D:/MoChengRT/assets/mocheng.obj");
    prepare();

    contexts.resize(1);
    contexts[Context_index::Graphic] = std::shared_ptr<RenderContext> { new RenderContext(m_device) };
    //
    std::vector<std::shared_ptr<ShaderModule>> graphic_shader_modules(Graphic_Pipeline::shader_stage_count);
    graphic_shader_modules[Graphic_Pipeline::VERT].reset(new ShaderModule("D:/MoChengRT/shader/post.vert.spv"));
    graphic_shader_modules[Graphic_Pipeline::FRAG].reset(new ShaderModule("D:/MoChengRT/shader/post.frag.spv"));
    contexts[Graphic]->prepare(graphic_shader_modules);
    contexts[Graphic]->prepare_descriptorset([]() {
        // Descriptor_Manager::Get_Singleton()
        //     ->Make_DescriptorSet(
        //         Context::Get_Singleton()
        //             ->get_compute_context()
        //             ->get_out_image(),
        //         0,
        //         vk::DescriptorType ::eCombinedImageSampler,
        //         vk::ShaderStageFlagBits::eFragment,
        //         Descriptor_Manager::Graphic);
    });
    contexts[Graphic]->prepare_pipeline(graphic_shader_modules);
    contexts[Graphic]->post_prepare();
}
std::shared_ptr<CommandBuffer> Blinn_phone_context::Begin_Frame()
{
    m_camera->move_update();

    return BeginGraphicFrame();
}
std::shared_ptr<CommandBuffer> Blinn_phone_context::BeginGraphicFrame()
{
    // get_device()->get_handle().waitIdle();
    auto& render_context = contexts[Graphic];
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        cmd->get_handle().bindPipeline(vk::PipelineBindPoint ::eGraphics, render_context->get_pipeline()->get_handle());
        // cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
        //                                      render_context->get_pipeline()->get_layout(),
        //                                      0,
        //                                      { //   Descriptor_Manager::Get_Singleton()->get_DescriptorSet(Descriptor_Manager::Compute)->get_handle()[0],
        //                                        Descriptor_Manager::Get_Singleton()
        //                                            ->get_DescriptorSet(Descriptor_Manager::Graphic)
        //                                            ->get_handle() },

        //                                      {});

        cmd->get_handle().bindIndexBuffer(Model::models[0]
                                              ->get_indices_buffer()
                                              ->get_handle(),
                                          0,
                                          vk::IndexType ::eUint32);
        cmd->get_handle().bindVertexBuffers(0,
                                            Model::models[0]->get_vertex_buffer()->get_handle(),
                                            { 0 });
        auto ss = Model::models[0]->get_vertex_count();
        cmd->get_handle()
            .drawIndexed(Model::models[0]->get_vertex_count(), 1, 0, 0, 0);

        render_context->record_command(cmd);
    }

    return cmd;
}
void Blinn_phone_context::EndFrame()
{
    auto ee = get_device()->Get_Graphic_queue().getCheckpointData2NV();
    for (auto& i : ee) {
        auto dd = vk::to_string(i.stage);
        auto rr = *(Vertex*)i.pCheckpointMarker;
        int eeee = 0;
    }
    EndGraphicFrame();
}

void Blinn_phone_context::EndGraphicFrame()
{
    auto& m_render_context = contexts[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}
}