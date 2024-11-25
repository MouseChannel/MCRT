#include "Helper/DescriptorSetTarget/ImageDescriptorTarget.hpp"
#include "Wrapper/DescriptorSet.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Image.hpp"
#include "Wrapper/Sampler.hpp"
namespace MCRT {

ImageDescriptorTarget::ImageDescriptorTarget(std::shared_ptr<Image> img,
                                             //    Which_Set _which_set,
                                             int binding_index,
                                             vk::ShaderStageFlagBits shader_stage,
                                             vk::DescriptorType type,
                                             DescriptorSet* descriptorSet)

    : DescriptorTargetBase(
          // _which_set,
          binding_index,
          shader_stage,
          type,
          descriptorSet)
{

    m_img_layouts.push_back(img->Get_image_layout());
    m_img_views.push_back(img->Get_Image_View());
}

ImageDescriptorTarget::ImageDescriptorTarget(std::vector<std::shared_ptr<Image>> imgs,
                                             //  Which_Set which_set,
                                             int binding_index,
                                             vk::ShaderStageFlagBits shader_stage,
                                             vk::DescriptorType type,
                                             DescriptorSet* descriptorSet)
    : DescriptorTargetBase(
          // _which_set,
          binding_index,
          shader_stage,
          type,
          descriptorSet)
{
    for (int i = 0; i < imgs.size(); i++) {
        m_img_layouts.push_back(imgs[i]->Get_image_layout());
        m_img_views.push_back(imgs[i]->Get_Image_View());
    }
}

ImageDescriptorTarget::ImageDescriptorTarget(std::vector<vk::ImageView> img_views,
                                             std::vector<vk::ImageLayout> img_layouts,
                                             int binding_index,
                                             vk::ShaderStageFlagBits shader_stage,
                                             vk::DescriptorType type,
                                             DescriptorSet* descriptorSet)
    : m_img_layouts(img_layouts)
    , m_img_views(img_views)
    , DescriptorTargetBase(
          binding_index,
          shader_stage,
          type,
          descriptorSet)
{
}

void ImageDescriptorTarget::Update()
{
    // if (m_type == vk::DescriptorType::eInputAttachment)

    std::vector<vk::DescriptorImageInfo> image_infos(m_img_views.size());
    assert(m_img_views.size() == m_img_layouts.size());
    for (int i = 0; i < m_img_views.size(); i++) {
        image_infos[i] = vk::DescriptorImageInfo()
                             .setImageLayout(m_img_layouts[i])
                             .setImageView(m_img_views[i])
                             .setSampler(Context::Get_Singleton()->get_sampler()->get_handle());
    }

    vk::WriteDescriptorSet writer;
    for (auto i = 0; i < m_descriptorSet->get_handle().size(); i++) {
        writer.setDescriptorType(m_type)
            .setPImageInfo(m_type == vk::DescriptorType::eInputAttachment ? &image_infos[i] : image_infos.data())
            .setDescriptorCount(m_type == vk::DescriptorType::eInputAttachment ? 1 : image_infos.size())
            // .setImageInfo(m_type == vk::DescriptorType::eInputAttachment ? image_infos : image_infos)
            .setDstBinding(m_binding_index)
            .setDstSet(m_descriptorSet->get_handle()[i]);

        // .setDstArrayElement(0);
        Context::Get_Singleton()
            ->get_device()
            ->get_handle()
            .updateDescriptorSets(writer, {});
        int rr = 0;
    }

   
}

}