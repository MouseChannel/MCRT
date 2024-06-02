

#include "Helper/DescriptorSetTarget/DescriptorTargetBase.hpp"
namespace MCRT {
class Image;
class ImageDescriptorTarget : public DescriptorTargetBase {
public:
    ImageDescriptorTarget(std::shared_ptr<Image> img,

                             int binding_index,
                             vk::ShaderStageFlagBits shader_stage,
                             vk::DescriptorType type,
                             std::shared_ptr<DescriptorSet> descriptorSet,
                             int descriptorSet_index = 0);

    ImageDescriptorTarget(std::vector<std::shared_ptr<Image>> imgs,

                             int binding_index,
                             vk::ShaderStageFlagBits shader_stage,
                             vk::DescriptorType type,
                             std::shared_ptr<DescriptorSet> descriptorSet,
                             int descriptorSet_index = 0);
    ImageDescriptorTarget(std::vector<vk::ImageView> img_views,
                             std::vector<vk::ImageLayout> img_layouts,
                             int binding_index,
                             vk::ShaderStageFlagBits shader_stage,
                             vk::DescriptorType type,
                             std::shared_ptr<DescriptorSet> descriptorSet,
                             int descriptorSet_index = 0);

    void Update() override;
    int get_descriptor_count() override
    {
        return m_img_views.size();
    }

private:
    // vk::ImageView m_img_view;
    // vk::ImageLayout m_layout;

    std::vector<vk::ImageView> m_img_views;
    std::vector<vk::ImageLayout> m_img_layouts;
};

}