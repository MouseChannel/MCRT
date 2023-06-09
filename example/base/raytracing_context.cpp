
#include "example/base/raytracing_context.hpp"
#include "Helper/Camera.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Helper/Model_Loader/gltf_loader.hpp"
#include "Rendering/Compute_context.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/RT_Context.hpp"
#include "Rendering/Render_Context.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Pipeline/Graphic_Pipeline.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/Texture.hpp"
#include "iostream"
#include "shader/Data_struct.h"

namespace MCRT {
ray_tracing_context::ray_tracing_context()
{
}
ray_tracing_context::~ray_tracing_context()
{
}
void ray_tracing_context::prepare(std::shared_ptr<Window> window)
{
    Context::prepare(window);
    std::vector<uint32_t> indices {
        0,
        1,
        2,
        0,
        3,
        1

    };

    index_buffer = Buffer::CreateDeviceBuffer(indices.data(), indices.size() * sizeof(indices[0]), vk::BufferUsageFlagBits::eIndexBuffer);

    std::vector<Vertex> vertex {
        Vertex {
            .pos { 1.0f, -1.0f, 0 },
            .texCoord { 1.0f, 1.0f } },
        Vertex {
            .pos { -1.0f, 1.0f, 0 },
            .texCoord { 0.0f, 0.0f } },
        Vertex {
            .pos { -1.0f, -1.0f, 0 },
            .texCoord { 0.0f, 1.0f } },
        Vertex {
            .pos { 1.0f, 1.0f, 0 },
            .texCoord { 1.0f, 0.0f } },
    };
    m_vertex_buffer = Buffer::CreateDeviceBuffer(vertex.data(), vertex.size() * sizeof(vertex[0]), vk::BufferUsageFlagBits::eVertexBuffer);
}
std::shared_ptr<CommandBuffer> ray_tracing_context::Begin_Frame()
{
    m_camera->move_update();

    return BeginGraphicFrame();
}
void ray_tracing_context::EndFrame()
{
    EndGraphicFrame();
}

std::shared_ptr<CommandBuffer> ray_tracing_context::BeginGraphicFrame()
{

    auto& render_context = contexts[Graphic];
    std::shared_ptr<CommandBuffer> cmd = render_context->BeginFrame();
    {
        cmd->get_handle().bindPipeline(vk::PipelineBindPoint ::eGraphics, render_context->get_pipeline()->get_handle());
        cmd->get_handle().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             render_context->get_pipeline()->get_layout(),
                                             0,
                                             { //
                                               Descriptor_Manager::Get_Singleton()
                                                   ->get_DescriptorSet(Descriptor_Manager::Graphic)
                                                   ->get_handle() },

                                             {});
        cmd->get_handle().bindIndexBuffer(index_buffer->get_handle(), 0, vk::IndexType ::eUint32);
        cmd->get_handle().bindVertexBuffers(0, { m_vertex_buffer->get_handle() }, { 0 });
        cmd->get_handle()
            .drawIndexed(6, 1, 0, 0, 0);
        render_context->record_command(cmd);
    }

    return cmd;
}

void ray_tracing_context::EndGraphicFrame()
{
    auto& m_render_context = contexts[Graphic];
    m_render_context->Submit();
    m_render_context->EndFrame();
}

}