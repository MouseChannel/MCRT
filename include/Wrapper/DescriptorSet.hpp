#pragma once
// #include "Wrapper/Base.hpp"

#include "Helper/DescriptorSetTarget/DescriptorSetTargetBase.hpp"
#include "Wrapper/Component.hpp"
// #include "Wrapper/Ray_Tracing/AS_top.hpp"
#include <map>
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>

namespace MCRT {
class DescriptorPool;

class Image;
class AccelerationStructure_Top;
// template <typename T>
// struct Data_Binding {
//     std::vector<std::shared_ptr<T>> data;
//     vk::DescriptorSetLayoutBinding binding;
//     Data_Binding() = default;
//     Data_Binding(std::vector<std::shared_ptr<T>> _data,
//                  vk::DescriptorSetLayoutBinding binding)
//         : data { _data }
//         , binding(binding)
//     {
//     }
// };
// class ImageDescriptorData {
// public:
//     ImageDescriptorData(vk::ImageView _image_view,
//                         vk::ImageLayout _image_layout)
//         : image_layout(_image_layout)
//         , image_view(_image_view)
//     {
//     }
//     vk::ImageView image_view;
//     vk::ImageLayout image_layout;
//     static std::shared_ptr<ImageDescriptorData> Create(vk::ImageView _image_view, vk::ImageLayout _image_layout)
//     {
//         return std::shared_ptr<ImageDescriptorData> { new ImageDescriptorData(_image_view, _image_layout) };
//     }
// };
class DescriptorSet : public Component<std::vector<vk::DescriptorSet>, DescriptorSet> {
public:
    DescriptorSet();
    // DescriptorSet(std::shared_ptr<DescriptorPool> pool, int size );
    ~DescriptorSet();
    void build(std::shared_ptr<DescriptorPool> pool, int size);

    void Add(std::shared_ptr<DescriptorSetTargetBase> descriptorSetTarget);
    // void Add(std::vector<std::shared_ptr<DescriptorSetTargetBase>> descriptorSetTargets);

    /////////////////////////////////////////////////////////////

    // template <typename T>
    // void Add(std::vector<std::shared_ptr<T>> data, vk::DescriptorSetLayoutBinding binding_data)
    // {
    //     add(data, binding_data);
    // }
    // // std::vector<vk::DescriptorSet>& get_handle() override;
    // void create(std::shared_ptr<DescriptorPool> pool, int size = 1);

    // void Update(int set_index)
    // {

    //     for (auto& [index, i] : m_images) {
    //         update(i.data, i.binding, set_index);
    //     }
    //     for (auto& [index, i] : _images) {
    //         update(i.data, i.binding, set_index);
    //     }
    //     for (auto& [index, i] : _buffers) {
    //         update(i.data, i.binding, set_index);
    //     }
    //     for (auto& [index, i] : _as) {
    //         update(i.data, i.binding, set_index);
    //     }
    // }

    [[nodiscard]] vk::DescriptorSetLayout& get_layout();
    // [[nodiscard]] auto& get_image_data()
    // {
    //     return images;
    // }
    // [[nodiscard]] auto& _get_image_data()
    // {
    //     return _images;
    // }
    // [[nodiscard]] auto& get_buffer_data()
    // {
    //     return _buffers;
    // }
    // [[nodiscard]] auto& get_as_data()
    // {
    //     return _as;
    // }

    bool check_dirty();

private:
    // void add(std::vector<std::shared_ptr<Image>> data, vk::DescriptorSetLayoutBinding binding_data);
    // void add(std::vector<std::shared_ptr<ImageDescriptorData>> data, vk::DescriptorSetLayoutBinding binding_data);
    // void add(std::vector<std::shared_ptr<Buffer>> data, vk::DescriptorSetLayoutBinding binding_data);
    // void add(std::vector<std::shared_ptr<AccelerationStructure_Top>> data, vk::DescriptorSetLayoutBinding binding_data);
    // void update(std::vector<std::shared_ptr<Buffer>> new_data,
    //             vk::DescriptorSetLayoutBinding binding_data,
    //             int set_index);

    // void update(std::vector<std::shared_ptr<Image>> new_data,
    //             vk::DescriptorSetLayoutBinding binding_data,
    //             int set_index);
    // void update(std::vector<std::shared_ptr<ImageDescriptorData>> new_data,
    //             vk::DescriptorSetLayoutBinding binding_data,
    //             int set_index);

    // void update(std::vector<std::shared_ptr<AccelerationStructure_Top>> new_data,
    //             vk::DescriptorSetLayoutBinding binding_data,
    //             int set_index);

    std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;
    std::map<int, vk::DescriptorSetLayoutBinding> m_layout_binding;
    vk::DescriptorSetLayout layout { VK_NULL_HANDLE };
    // std::shared_ptr<DescriptorPool> pool;

    // data

    // std::vector<Data_Binding<Image>> images;
    // std::vector<Data_Binding<Buffer>> buffers;
    // std::vector<Data_Binding<AccelerationStructure_Top>> as;

    // std::map<uint32_t, Data_Binding<Image>> _images;
    // std::map<uint32_t, Data_Binding<ImageDescriptorData>> m_images;

    // std::unordered_map<int, Data_Binding<Buffer>> _buffers;
    // std::unordered_map<int, Data_Binding<AccelerationStructure_Top>> _as;
};
} // namespace MoCheng3D