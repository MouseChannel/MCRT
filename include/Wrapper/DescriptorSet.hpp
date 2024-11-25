#pragma once
// #include "Wrapper/Base.hpp"

#include "Helper/DescriptorSetTarget/DescriptorTargetBase.hpp"
#include "Wrapper/Component.hpp"
// #include "Wrapper/Ray_Tracing/AS_top.hpp"
#include <map>
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>

namespace MCRT {
class DescriptorPool;

class Image;
class AccelerationStructure_Top;
class Buffer;
class RenderTarget;
class DescriptorSet : public Component<std::vector<vk::DescriptorSet>, DescriptorSet> {
public:
    DescriptorSet();
    // DescriptorSet(std::shared_ptr<DescriptorPool> pool, int size );
    ~DescriptorSet();
    void build(std::shared_ptr<DescriptorPool> pool, int size);
    void Add(std::shared_ptr<DescriptorTargetBase> descriptorSetTarget);

    void Update();
    [[nodiscard]] vk::DescriptorSetLayout get_layout();

    bool check_dirty();
    std::map<vk::DescriptorType, uint32_t> GetTypeMap();

    void AddBufferDescriptorTarget(std::shared_ptr<Buffer> buf,
                                   int binding_index,
                                   vk::ShaderStageFlags shader_stage,
                                   vk::DescriptorType type);
    void AddImageDescriptorTarget(std::shared_ptr<Image> img,
                                  int binding_index,
                                  vk::ShaderStageFlagBits shader_stage,
                                  vk::DescriptorType type);
    void AddImageDescriptorTarget(std::vector<std::shared_ptr<Image>> imgs,

                                  int binding_index,
                                  vk::ShaderStageFlagBits shader_stage,
                                  vk::DescriptorType type);
    void AddImageDescriptorTarget(std::vector<vk::ImageView> img_views,
                                  std::vector<vk::ImageLayout> img_layouts,
                                  int binding_index,
                                  vk::ShaderStageFlagBits shader_stage,
                                  vk::DescriptorType type);
    void AddInputImageDescriptorTarget(
        std::vector<std::vector<std::shared_ptr<RenderTarget>>>& all_rendertarget,
        int input_source,
        int binding_index);

    std::map<int, std::shared_ptr<DescriptorTargetBase>> m_descriptorTargetMap;
    std::map<int, int> m_attachent_bind_map;
    std::shared_ptr<DescriptorPool> pool;
private:
    std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;
    std::map<int, vk::DescriptorSetLayoutBinding> m_layout_binding;
    vk::DescriptorSetLayout layout { VK_NULL_HANDLE };
};
} // namespace MCRT