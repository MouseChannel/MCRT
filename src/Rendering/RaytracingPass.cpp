#include "Rendering/RaytracingPass.hpp"
#include "Helper/math.hpp"

// #include "Helper/Ray_Tracing/RT_Manager.hpp"
#include "Helper/Camera.hpp"
#include "Helper/Debugger.hpp"
#include "Helper/DescriptorManager.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "Helper/math.hpp"
#include "Rendering/BasePass.hpp"
#include "Rendering/Context.hpp"
#include "Rendering/Model.hpp"
#include "Rendering/Render_Target/Color_Render_Target.hpp"
#include "Rendering/Render_Target/Depth_Render_Target.hpp"
#include "example/base/shaders/ray_tracing/Data_struct.h"

#include "Rendering/Render_Target/Render_Target.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/FrameBuffer.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Pipeline/RT_pipeline.hpp"
#include "Wrapper/Ray_Tracing/AS_Builder.hpp"
#include "Wrapper/RenderPass.hpp"
#include "Wrapper/Shader_module.hpp"
#include "Wrapper/SwapChain.hpp"
#include "shaders/Data_struct.h"

namespace MCRT {
RaytracingPass::RaytracingPass(std::shared_ptr<Device> device)
    : m_device(device)
{
}

RaytracingPass::RaytracingPass()
{
}

std::shared_ptr<Pipeline_base> RaytracingPass::get_pipeline()
{
    return m_rt_pipeline;
}

std::shared_ptr<Image> RaytracingPass::get_out_image()
{
    return m_out_image;
}

void RaytracingPass::build_accelerate_structure()
{

    Mesh::update_accelerate_structure_data();
    for (auto obj : Mesh::meshs) {
        AS_Builder::Get_Singleton()->add_blas_obj(obj);
    }

    AS_Builder::Get_Singleton()->build_blas();
    AS_Builder::Get_Singleton()->build_tlas();
}

void RaytracingPass::create_shader_bind_table()
{

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#else
    auto device_properties = Context::Get_Singleton()
                             ->get_device()
                             ->Get_Physical_device()
                             .getProperties2<vk::PhysicalDeviceProperties2,
                                             vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    auto& rt_pipeline_properties = device_properties.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
    // set up sbt data
    auto missCount{ miss_shader_count };
    auto hitCount{ hit_shader_count };
    // auto anyhitCount{ anyhit_shader_count };
    auto handleCount = 1 + missCount + hitCount ;
    uint32_t handleSize = rt_pipeline_properties.shaderGroupHandleSize;

    uint32_t handleSizeAligned = align_up(handleSize, rt_pipeline_properties.shaderGroupHandleAlignment);

    m_rgenRegion.stride = align_up(handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    m_rgenRegion.size = m_rgenRegion.stride; // The size member of pRayGenShaderBindingTable must be equal to its stride member
    m_missRegion.stride = handleSizeAligned;
    m_missRegion.size = align_up(missCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    m_hitRegion.stride = handleSizeAligned;
    m_hitRegion.size = align_up(hitCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    // if (anyhitCount > 0) {
    //
    //     m_anyhitRegion.stride = handleSizeAligned;
    //     m_anyhitRegion.size = align_up(anyhitCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
    // }

    // Get the shader group handles
    uint32_t dataSize = handleCount * handleSize;
    std::vector<uint8_t> handles(dataSize);
    // auto rt_pipeline = Context::Get_Singleton()->get_rt_pipeline();
    auto res = Context::Get_Singleton()
               ->get_device()
               ->get_handle()
               .getRayTracingShaderGroupHandlesKHR(m_rt_pipeline->get_handle(),
                                                   0,
                                                   handleCount,
                                                   dataSize,
                                                   handles.data());
    if (res != vk::Result::eSuccess) {
        throw std::runtime_error("failed to getRayTracingShaderGroupHandles");
    }

    // Allocate a buffer for storing the SBT.
    auto sbtSize = m_rgenRegion.size + m_missRegion.size + m_hitRegion.size + m_callRegion.size;
    m_SBT_buffer = Buffer::create_buffer(nullptr, sbtSize, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR);
    Context::Get_Singleton()->get_debugger()->set_name(m_SBT_buffer, "sbt_buffer");

    // allocate all kinds of sbt_buffer
    auto sbt_buffer_usge = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR;
    m_SBT_buffer_rgen = Buffer::create_buffer(nullptr, m_rgenRegion.size, sbt_buffer_usge);
    m_SBT_buffer_rmiss = Buffer::create_buffer(nullptr, m_missRegion.size, sbt_buffer_usge);
    m_SBT_buffer_rhit = Buffer::create_buffer(nullptr, m_hitRegion.size, sbt_buffer_usge);
     

    // assign all kinds of handle data

    std::vector<uint8_t> rgen_handles;
    rgen_handles.assign(handles.begin(), handles.begin() + 1 * handleSize);
    std::vector<uint8_t> rmiss_handles;
    auto rmiss_begin = handles.begin() + 1 * handleSize;
    rmiss_handles.assign(rmiss_begin, rmiss_begin + missCount * handleSize);
    std::vector<uint8_t> rhit_handles;
    auto rhit_begin = handles.begin() + (1 + missCount) * handleSize;
    rhit_handles.assign(rhit_begin, rhit_begin + hitCount * handleSize);
    // std::vector<uint8_t> rahit_handles;
    // auto rahit_begin = handles.begin() + (1 + missCount + hitCount) * handleSize;
    // rahit_handles.assign(rahit_begin, rahit_begin + anyhitCount * handleSize);
    //
    m_rgenRegion.setDeviceAddress(m_SBT_buffer_rgen->get_address());
    m_missRegion.setDeviceAddress(m_SBT_buffer_rmiss->get_address());
    m_hitRegion.setDeviceAddress(m_SBT_buffer_rhit->get_address());
    // m_anyhitRegion.setDeviceAddress(m_SBT_buffer_rahit->get_address());

    m_SBT_buffer_rgen->Update(rgen_handles.data(), rgen_handles.size());
    m_SBT_buffer_rmiss->Update(rmiss_handles.data(), rmiss_handles.size());
    m_SBT_buffer_rhit->Update(rhit_handles.data(), rhit_handles.size());
    // m_SBT_buffer_rahit->Update(rahit_handles.data(), rahit_handles.size());
#endif
}

void RaytracingPass::prepare_descriptorset(std::function<void()> prepare_func)
{
    prepare_func();
    //        Descriptor_Manager::Get_Singleton()
    //                ->Make_DescriptorSet(camera_data,
    //                                     (int) Global_Binding::e_camera,
    //                                     Descriptor_Manager::Global);
    //
    //        Descriptor_Manager::Get_Singleton()
    //                ->Make_DescriptorSet(obj_data_address,
    //                                     (int) Global_Binding::e_obj_addresses,
    //                                     Descriptor_Manager::Global);
    //
    //        Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Ray_Tracing);
    //
    //        Descriptor_Manager::Get_Singleton()->CreateDescriptorPool(Descriptor_Manager::Global);
    //        Descriptor_Manager::Get_Singleton()->update_descriptor_set(Descriptor_Manager::Ray_Tracing);
    //
    //        Descriptor_Manager::Get_Singleton()->update_descriptor_set(Descriptor_Manager::Global);
    ////
    get_descriptor_manager()
        ->Make_DescriptorSet(camera_data,
                             (int)Global_Binding::e_camera,
                             DescriptorManager::Global);

    get_descriptor_manager()
        ->Make_DescriptorSet(obj_data_address,
                             (int)Global_Binding::e_obj_addresses,
                             DescriptorManager::Global);

    get_descriptor_manager()->CreateDescriptorPool(DescriptorManager::Ray_Tracing);

    get_descriptor_manager()->CreateDescriptorPool(DescriptorManager::Global);
    get_descriptor_manager()->update_descriptor_set(DescriptorManager::Ray_Tracing);

    get_descriptor_manager()->update_descriptor_set(DescriptorManager::Global);
}

void RaytracingPass::prepare_pipeline(std::vector<std::shared_ptr<ShaderModule>> shader_modules,
                                      std::vector<std::shared_ptr<DescriptorSet>> sets,
                                      int push_constants_size)
{
    m_rt_pipeline.reset(new RT_Pipeline(shader_modules, sets, push_constants_size));
}

void RaytracingPass::create_uniform_buffer()
{
    // V_P_Matrix vp_matrix_data {
    //     Context::Get_Singleton()->get_camera()->Get_v_matrix(),
    //     Context::Get_Singleton()->get_camera()->Get_p_matrix()
    // };

    Camera_data _camera_data{
        // .camera_pos { glm::vec3 { 1, 2, 3 } },
        // .s = 1,
        // .rr = 4,
        .viewInverse{
            glm::inverse(Context::Get_Singleton()
                         ->get_camera()
                         ->Get_v_matrix()) },

        .camera_pos{
            Context::Get_Singleton()
            ->get_camera()
            ->get_pos(),
            0,
        },
        .camera_front{
            Context::Get_Singleton()
            ->get_camera()
            ->get_front(),
            0,
        },
        .fov_angel = Context::Get_Singleton()
                     ->get_camera()
                     ->get_fov_angel(),
    };

    camera_data = UniformManager::make_uniform({ _camera_data },
                                               vk::ShaderStageFlagBits::eRaygenKHR |
                                               vk::ShaderStageFlagBits::eClosestHitKHR,
                                               vk::DescriptorType::eUniformBuffer);

    m_objs_address.resize(Mesh::meshs.size());
    for (auto& obj : Mesh::meshs) {

        m_objs_address[obj->get_instance_index()] = Address{
            .triangle_count = obj->get_vertex_count() / 3,

            .vertexAddress = obj->get_vertex_buffer()
                                ->get_address(),
            .indexAddress = obj->get_indices_buffer()
                               ->get_address(),
            .materialAddress = obj->get_material_buffer()
                                  ->get_address()
        };
    }

    obj_data_address = UniformManager::make_uniform(m_objs_address,
                                                    vk::ShaderStageFlagBits::eClosestHitKHR,
                                                    vk::DescriptorType::eStorageBuffer);
}

void RaytracingPass::prepare()
{

    build_accelerate_structure();
    create_offscreen_image();
    create_uniform_buffer();

    m_command_buffer.reset(new CommandBuffer);
}

void RaytracingPass::create_offscreen_image()
{
    extent2d = Context::Get_Singleton()->get_extent2d();
    // std::cout << extent2d.width << extent2d.height << std::endl;
    //    Context::Get_Singleton()->ge
    m_out_image.reset(new Image(extent2d.width,
                                extent2d.height,
                                vk::Format::eR32G32B32A32Sfloat,
                                vk::ImageType::e2D,
                                vk::ImageTiling::eOptimal,
                                vk::ImageUsageFlagBits::eStorage |
                                vk::ImageUsageFlagBits::eSampled,
                                vk::ImageAspectFlagBits::eColor,
                                vk::SampleCountFlagBits::e1));

    m_out_image->SetImageLayout(vk::ImageLayout::eGeneral,
                                vk::AccessFlagBits::eNone,
                                vk::AccessFlagBits::eNone,
                                vk::PipelineStageFlagBits::eTopOfPipe,
                                vk::PipelineStageFlagBits::eBottomOfPipe);

    // gbuffer
    m_gbuffer.resize((int)Gbuffer_Index::gbuffer_count);
    m_gbuffer[(int)Gbuffer_Index::position].reset(new Image(extent2d.width,
                                                            extent2d.height,
                                                            vk::Format::eR32G32B32A32Sfloat,
                                                            vk::ImageType::e2D,
                                                            vk::ImageTiling::eOptimal,
                                                            vk::ImageUsageFlagBits::eStorage,
                                                            vk::ImageAspectFlagBits::eColor,
                                                            vk::SampleCountFlagBits::e1));

    m_gbuffer[(int)Gbuffer_Index::position]->SetImageLayout(vk::ImageLayout::eGeneral,
                                                            vk::AccessFlagBits::eNone,
                                                            vk::AccessFlagBits::eNone,
                                                            vk::PipelineStageFlagBits::eTopOfPipe,
                                                            vk::PipelineStageFlagBits::eBottomOfPipe);

    m_gbuffer[(int)Gbuffer_Index::normal].reset(new Image(extent2d.width,
                                                          extent2d.height,
                                                          vk::Format::eR32G32B32A32Sfloat,
                                                          vk::ImageType::e2D,
                                                          vk::ImageTiling::eOptimal,
                                                          vk::ImageUsageFlagBits::eStorage,
                                                          vk::ImageAspectFlagBits::eColor,
                                                          vk::SampleCountFlagBits::e1));

    m_gbuffer[(int)Gbuffer_Index::normal]
        ->SetImageLayout(vk::ImageLayout::eGeneral,
                         vk::AccessFlagBits::eNone,
                         vk::AccessFlagBits::eNone,
                         vk::PipelineStageFlagBits::eTopOfPipe,
                         vk::PipelineStageFlagBits::eBottomOfPipe);
    // end gbuffer
}

void RaytracingPass::post_prepare()
{
    create_shader_bind_table();
}

void RaytracingPass::re_create()
{
    create_offscreen_image();
}

void RaytracingPass::update_ubo(std::shared_ptr<CommandBuffer> cmd)
{

    // Ensure that the modified UBO is not visible to previous frames.
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto before_barrier = vk::BufferMemoryBarrier {}
                              .setSrcAccessMask(vk::AccessFlagBits::eShaderRead)
                              .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                              .setBuffer(camera_data->buffer->get_handle())
                              .setSize(VK_WHOLE_SIZE);
    //        auto before_barrier = vk::BufferMemoryBarrier2{}
    //                .setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR |
    //                                 vk::PipelineStageFlagBits2::eVertexShader)
    //                .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
    //                .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
    //                .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite)
    //                .setBuffer(camera_data->buffer->get_handle())
    //                .setSize(VK_WHOLE_SIZE);

    cmd->get_handle()
        .pipelineBarrier(vk::PipelineStageFlagBits::eRayTracingShaderKHR |
                             vk::PipelineStageFlagBits::eVertexShader,
                         vk::PipelineStageFlagBits::eTransfer,
                         {},
                         {},
                         before_barrier,
                         {});
#else

    auto before_barrier = vk::BufferMemoryBarrier2{}
                          .setSrcStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR | vk::PipelineStageFlagBits2::eVertexShader)
                          .setSrcAccessMask(vk::AccessFlagBits2::eShaderRead)
                          .setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
                          .setDstAccessMask(vk::AccessFlagBits2::eTransferWrite)
                          .setBuffer(camera_data->buffer->get_handle())
                          .setSize(VK_WHOLE_SIZE);

    cmd->get_handle()
       .pipelineBarrier2(vk::DependencyInfo()
           .setBufferMemoryBarriers(before_barrier));

#endif
    cmd->get_handle()
       .updateBuffer<Camera_data>(
           camera_data->buffer->get_handle(),
           0,
           Camera_data{
               //    .camera_pos { pos },
               .viewInverse{
                   glm::inverse(Context::Get_Singleton()
                                ->get_camera()
                                ->Get_v_matrix()) },

               .camera_pos{
                   Context::Get_Singleton()
                   ->get_camera()
                   ->get_pos(),
                   1 },
               .camera_front{
                   Context::Get_Singleton()
                   ->get_camera()
                   ->get_front(),
                   0 },
               .fov_angel = Context::Get_Singleton()
                            ->get_camera()
                            ->get_fov_angel(),
           });
    // Making sure the updated UBO will be visible.

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto after_barrier = vk::BufferMemoryBarrier {}
                             .setBuffer(camera_data->buffer->get_handle())
                             //                .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
                             .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                             //                .setDstStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR |
                             //                                 vk::PipelineStageFlagBits2::eVertexShader)
                             .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                             .setSize(VK_WHOLE_SIZE);

    cmd->get_handle()
        .pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                         vk::PipelineStageFlagBits::eRayTracingShaderKHR |
                             vk::PipelineStageFlagBits::eVertexShader,
                         {},
                         {},
                         after_barrier,
                         {});
#else

    auto after_barrier = vk::BufferMemoryBarrier2{}
                         .setBuffer(camera_data->buffer->get_handle())
                         .setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
                         .setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
                         .setDstStageMask(vk::PipelineStageFlagBits2::eRayTracingShaderKHR |
                             vk::PipelineStageFlagBits2::eVertexShader)
                         .setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
                         .setSize(VK_WHOLE_SIZE);

    cmd->get_handle()
       .pipelineBarrier2(vk::DependencyInfo()
           .setBufferMemoryBarriers(after_barrier));
#endif
}

void RaytracingPass::record_command(std::shared_ptr<CommandBuffer> cmd)
{
    update_ubo(cmd);
    auto extent = Context::Get_Singleton()->get_extent2d();
    cmd->get_handle().traceRaysKHR(m_rgenRegion,
                                   m_missRegion,
                                   m_hitRegion,
                                   m_callRegion,
                                   extent.width,
                                   extent.height,
                                   1);

    Context::Get_Singleton()
        ->get_debugger()
        ->set_name(cmd, "ray tracing command_buffer");
}

std::shared_ptr<CommandBuffer> RaytracingPass::BeginFrame()
{
    m_command_buffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    return m_command_buffer;
}

void RaytracingPass::Submit()
{
    m_command_buffer->End();

    auto queue = Context::Get_Singleton()->get_device()->Get_Graphic_queue();
    vk::SubmitInfo submit_info;
    submit_info.setCommandBuffers(m_command_buffer->get_handle());
    queue.submit(submit_info);
    // queue.waitIdle();
}

void RaytracingPass::EndFrame()
{
    // m_command_buffer->End();
}

}
