// #pragma once
// #include "Helper/Debugger.hpp"
// #include "Helper/DescriptorManager.hpp"
// #include "Helper/math.hpp"
// #include "Wrapper/Buffer.hpp"
// #include "Wrapper/Image.hpp"
// #include "Wrapper/Pipeline/RT_pipeline.hpp"
// #include "Wrapper/Ray_Tracing/AS_Builder.hpp"
// #include "Wrapper/Ray_Tracing/AS_base.hpp"
// #include "Wrapper/Ray_Tracing/AS_top.hpp"

// namespace MCRT {

// enum RT_Binding {
//     e_tlas = 0,
//     e_out_image = 1

// };
// static std::shared_ptr<Image> rt_out_image;
// static void create_rt_descriptor_set()
// {
//     Descriptor_Manager::Get_Singleton()
//         ->Make_DescriptorSet(AS_Builder::Get_Singleton()->get_tlas(),
//                              RT_Binding::e_tlas,
//                              vk::DescriptorType::eAccelerationStructureKHR,
//                              vk::ShaderStageFlagBits::eRaygenKHR | vk::ShaderStageFlagBits::eClosestHitKHR);
//     rt_out_image.reset(new Image(800,
//                                  800,
//                                  vk::Format::eR32G32B32A32Sfloat,
//                                  vk::ImageType::e2D,
//                                  vk::ImageTiling::eOptimal,
//                                  vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
//                                  vk::ImageAspectFlagBits::eColor,
//                                  vk::SampleCountFlagBits::e8));
//     Context::Get_Singleton()->get_debugger()->set_name(rt_out_image, "out_image");
//     rt_out_image->SetImageLayout(vk::ImageLayout::eGeneral,
//                                  vk::AccessFlagBits::eNone,
//                                  vk::AccessFlagBits::eNone,
//                                  vk::PipelineStageFlagBits ::eTopOfPipe,
//                                  vk::PipelineStageFlagBits::eBottomOfPipe);

//     Descriptor_Manager::Get_Singleton()
//         ->Make_DescriptorSet(rt_out_image,
//                              RT_Binding::e_out_image,
//                              vk::DescriptorType::eStorageImage,
//                              vk::ShaderStageFlagBits::eRaygenKHR);

//     Descriptor_Manager::Get_Singleton()->CreateDescriptorPool();
//     Descriptor_Manager::Get_Singleton()->CreateUpdateDescriptorSet();
// }
// static vk::StridedDeviceAddressRegionKHR m_rgenRegion;
// static vk::StridedDeviceAddressRegionKHR m_missRegion;
// static vk::StridedDeviceAddressRegionKHR m_hitRegion;
// static vk::StridedDeviceAddressRegionKHR m_callRegion;
// static std::shared_ptr<Buffer> m_SBT_buffer;

// static std::shared_ptr<Buffer> m_SBT_buffer_rgen;

// static std::shared_ptr<Buffer> m_SBT_buffer_rmiss;

// static std::shared_ptr<Buffer> m_SBT_buffer_rhit;

// static void createRt_shader_binding_table()
// {
//     // m_rgenRegion.setDeviceAddress(vk::DeviceAddress deviceAddress_)
//     vk::PhysicalDeviceProperties2 a_;
//     vk::PhysicalDeviceRayTracingPipelinePropertiesKHR b_;
//     vk::StructureChain<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>
//         chain { a_, b_ };
//     auto device_properties = Context::Get_Singleton()
//                                  ->get_device()
//                                  ->Get_Physical_device()
//                                  .getProperties2<vk::PhysicalDeviceProperties2,
//                                                  vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
//     auto& rt_pipeline_properties = device_properties.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
//     // set up sbt data
//     uint32_t missCount { 1 };
//     uint32_t hitCount { 1 };
//     auto handleCount = 1 + missCount + hitCount;
//     uint32_t handleSize = rt_pipeline_properties.shaderGroupHandleSize;

//     uint32_t handleSizeAligned = align_up(handleSize, rt_pipeline_properties.shaderGroupHandleAlignment);

//     m_rgenRegion.stride = align_up(handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
//     m_rgenRegion.size = m_rgenRegion.stride; // The size member of pRayGenShaderBindingTable must be equal to its stride member
//     m_missRegion.stride = handleSizeAligned;
//     m_missRegion.size = align_up(missCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);
//     m_hitRegion.stride = handleSizeAligned;
//     m_hitRegion.size = align_up(hitCount * handleSizeAligned, rt_pipeline_properties.shaderGroupBaseAlignment);

//     // Get the shader group handles
//     uint32_t dataSize = handleCount * handleSize;
//     std::vector<uint8_t> handles(dataSize);
//     auto rt_pipeline = Context::Get_Singleton()->get_rt_pipeline();
//     auto res = Context::Get_Singleton()
//                    ->get_device()
//                    ->get_handle()
//                    .getRayTracingShaderGroupHandlesKHR(rt_pipeline->get_handle(),
//                                                        0,
//                                                        handleCount,
//                                                        dataSize,
//                                                        handles.data());
//     if (res != vk::Result::eSuccess) {
//         throw std::runtime_error("failed to getRayTracingShaderGroupHandles");
//     }

//     // Allocate a buffer for storing the SBT.
//     auto sbtSize = m_rgenRegion.size + m_missRegion.size + m_hitRegion.size + m_callRegion.size;
//     m_SBT_buffer = Buffer::create_buffer(nullptr, sbtSize, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR);
//     Context::Get_Singleton()->get_debugger()->set_name(m_SBT_buffer, "sbt_buffer");

//     // allocate all kinds of sbt_buffer
//     auto sbt_buffer_usge = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR;
//     m_SBT_buffer_rgen = Buffer::create_buffer(nullptr, m_rgenRegion.size, sbt_buffer_usge);
//     m_SBT_buffer_rmiss = Buffer::create_buffer(nullptr, m_missRegion.size, sbt_buffer_usge);
//     m_SBT_buffer_rhit = Buffer::create_buffer(nullptr, m_hitRegion.size, sbt_buffer_usge);
//     // assign all kinds of handle data
//     std::vector<uint8_t> rgen_handles;
//     rgen_handles.assign(handles.begin(), handles.begin() + 1 * handleSize);
//     std::vector<uint8_t> rmiss_handles;
//     auto rmiss_begin = handles.begin() + 1 * handleSize;
//     rmiss_handles.assign(rmiss_begin, rmiss_begin + missCount * handleSize);
//     std::vector<uint8_t> rhit_handles;
//     auto rhit_begin = handles.begin() + (1 + missCount) * handleSize;
//     rhit_handles.assign(rhit_begin, rhit_begin + hitCount * handleSize);
//     //
//     m_rgenRegion.setDeviceAddress(m_SBT_buffer_rgen->get_address());
//     m_missRegion.setDeviceAddress(m_SBT_buffer_rmiss->get_address() + m_rgenRegion.size);
//     m_hitRegion.setDeviceAddress(m_SBT_buffer_rhit->get_address() + m_rgenRegion.size + m_missRegion.size);

//     // TODO {missCount or hitcount} >0
//     m_SBT_buffer_rgen->Update(rgen_handles.data(), handleSize);
//     m_SBT_buffer_rmiss->Update(rmiss_handles.data(), handleSize);
//     m_SBT_buffer_rhit->Update(rhit_handles.data(), handleSize);

//     // memc
//     // m_SBT_buffer_rgen->Update(void *data, size_t size)
// }
// }