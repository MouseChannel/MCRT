#include "Wrapper/DescriptorSet.hpp"
#include "Helper/DescriptorSetTarget/BufferDescriptorTarget.hpp"
#include "Helper/DescriptorSetTarget/ImageDescriptorTarget.hpp"

#include "Rendering/GraphicContext.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/DescriptorPool.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Ray_Tracing/AS_top.hpp"
#include "Wrapper/Sampler.hpp"
#include "Wrapper/SwapChain.hpp"
#include <map>

namespace MCRT {
DescriptorSet::DescriptorSet()
{
}

void DescriptorSet::build(std::shared_ptr<DescriptorPool> pool, int size)
{
    vk::DescriptorSetAllocateInfo allocate_info;
    // auto swapchain_size = Get_Context_Singleton()->get_swapchain()->Get_Swapchain_Image_size();
    std::vector<vk::DescriptorSetLayout> layouts(size);
    for (int i = 0; i < layouts.size(); i++) {
        layouts[i] = get_layout();
    }
    allocate_info.setDescriptorPool(pool->get_handle())
        .setSetLayouts(layouts)

        .setDescriptorSetCount(size);
    m_handle = Get_Context_Singleton()
                   ->get_device()
                   ->get_handle()
                   .allocateDescriptorSets(allocate_info);
    Update();
}
DescriptorSet::~DescriptorSet()
{

    Context::Get_Singleton()
        ->get_device()
        ->get_handle()
        .destroyDescriptorSetLayout(layout);
}
void DescriptorSet::Update()
{
    for (auto& [k, v] : m_descriptorTargetMap)
        v->Update();
}

void DescriptorSet::Add(std::shared_ptr<DescriptorTargetBase> descriptorSetTarget)
{
    m_layout_binding[descriptorSetTarget->get_binding_index()] =
        vk::DescriptorSetLayoutBinding()
            .setBinding(descriptorSetTarget->get_binding_index())

            .setDescriptorCount(descriptorSetTarget->get_type() == vk::DescriptorType::eInputAttachment ? 1 : descriptorSetTarget->get_descriptor_count())

            .setStageFlags(descriptorSetTarget->get_shader_stage())
            .setDescriptorType(descriptorSetTarget->get_type());

    m_descriptorTargetMap[descriptorSetTarget->get_binding_index()] = descriptorSetTarget;
}

vk::DescriptorSetLayout DescriptorSet::get_layout()
{

    if (check_dirty()) {

        layout_bindings.resize(m_layout_binding.size());
        std::transform(m_layout_binding.begin(),
                       m_layout_binding.end(),
                       layout_bindings.begin(),
                       [](auto& item) {
                           return item.second;
                       });
        if (layout) {
            Context::Get_Singleton()
                ->get_device()
                ->get_handle()
                .destroyDescriptorSetLayout(layout);
        }
        layout = Context::Get_Singleton()
                     ->get_device()
                     ->get_handle()
                     .createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo().setBindings(layout_bindings));
    }
    // }
    return layout;
}
bool DescriptorSet::check_dirty()
{
    if (layout_bindings.empty()) {
        return true;
    }
    if (layout_bindings.size() != m_layout_binding.size()) {
        return true;
    }
    for (int i = 0; i < layout_bindings.size(); i++) {
        auto& old = layout_bindings[i];
        auto& cur = m_layout_binding[old.binding];
        if (old != cur) {
            return true;
        }
    }
    return false;
}
std::map<vk::DescriptorType, uint32_t> DescriptorSet::GetTypeMap()
{
    std::map<vk::DescriptorType, uint32_t> type_map;

    for (auto& [key, value] : m_descriptorTargetMap) {
        type_map[value->get_type()] += value->get_descriptor_count();
    }
    return type_map;
}
void DescriptorSet::AddBufferDescriptorTarget(std::shared_ptr<Buffer> buf,
                                              int binding_index,
                                              vk::ShaderStageFlags shader_stage,
                                              vk::DescriptorType type)
{
    auto target = std::make_shared<BufferDescriptorTarget>(
        buf,
        binding_index,
        shader_stage,
        type,
        this);
    Add(target);
}
void DescriptorSet::AddImageDescriptorTarget(std::shared_ptr<Image> img,
                                             int binding_index,
                                             vk::ShaderStageFlagBits shader_stage,
                                             vk::DescriptorType type)
{
    auto target = std::make_shared<ImageDescriptorTarget>(
        img,
        binding_index,
        shader_stage,
        type,
        this);
    Add(target);
}
void DescriptorSet::AddImageDescriptorTarget(std::vector<std::shared_ptr<Image>> imgs,

                                             int binding_index,
                                             vk::ShaderStageFlagBits shader_stage,
                                             vk::DescriptorType type)
{
    auto target = std::make_shared<ImageDescriptorTarget>(
        imgs,
        binding_index,
        shader_stage,
        type,
        this);
    Add(target);
}
void DescriptorSet::AddImageDescriptorTarget(std::vector<vk::ImageView> img_views,
                                             std::vector<vk::ImageLayout> img_layouts,
                                             int binding_index,
                                             vk::ShaderStageFlagBits shader_stage,
                                             vk::DescriptorType type)
{
    auto target = std::make_shared<ImageDescriptorTarget>(
        img_views,
        img_layouts,
        binding_index,
        shader_stage,
        type,
        this);
    Add(target);
}

void DescriptorSet::AddInputImageDescriptorTarget( 
    std::vector<std::vector<std::shared_ptr<RenderTarget>>> &all_rendertarget,
    int input_source,
    int binding_index)
{
    std::vector<vk::ImageView> input_image_view;
    std::vector<vk::ImageLayout> img_layouts;
    for (int i = 0; i < all_rendertarget.size(); i++) {
        auto input_renderTarget = all_rendertarget[i][input_source];
        input_image_view.push_back(input_renderTarget->Get_Image()->Get_Image_View());
        img_layouts.push_back(input_renderTarget->get_inputLayout());
    }
    auto target = std::make_shared<ImageDescriptorTarget>(
        input_image_view,
        img_layouts,
        binding_index,
        vk::ShaderStageFlagBits::eFragment,
        vk::DescriptorType::eInputAttachment,
        this);

    Add(target);
    m_attachent_bind_map[binding_index] = input_source;
}
}