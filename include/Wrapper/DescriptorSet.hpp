#pragma once
// #include "Wrapper/Base.hpp"
#include "Wrapper/Component.hpp"
// #include "Wrapper/Ray_Tracing/AS_top.hpp"
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>

namespace MCRT {
class DescriptorPool;
class Buffer;
class Image;
class AccelerationStructure_Top;
template <typename T>
struct Data_Binding {
    std::vector<std::shared_ptr<T>> data;
    vk::DescriptorSetLayoutBinding binding;
    Data_Binding() = default;
    Data_Binding(std::vector<std::shared_ptr<T>> _data,
                 vk::DescriptorSetLayoutBinding binding)
        : data { _data }
        , binding(binding)
    {
    }
};
class DescriptorSet : public Component<std::vector<vk::DescriptorSet>, DescriptorSet> {
public:
    DescriptorSet();
    ~DescriptorSet();
    template <typename T>
    void Add(std::vector<std::shared_ptr<T>> data, vk::DescriptorSetLayoutBinding binding_data)
    {
        add(data, binding_data);
    }
    // std::vector<vk::DescriptorSet>& get_handle() override;
    void create(std::shared_ptr<DescriptorPool> pool, int size = 1);
    //    void create_descriptor_pool();
    void Update(int set_index)
    {
        // for (auto& i : images) {
        //     update(i.data, i.binding);
        // }
        // for (auto& i : buffers) {
        //     update(i.data, i.binding);
        // }
        // for (auto& i : as) {
        //     update(i.data, i.binding);
        // }
        for (auto& [index, i] : _images) {
            update(i.data, i.binding, set_index);
        }
        for (auto& [index, i] : _buffers) {
            update(i.data, i.binding, set_index);
        }
        for (auto& [index, i] : _as) {
            update(i.data, i.binding, set_index);
        }
    }

    [[nodiscard]] vk::DescriptorSetLayout get_layout();
    // [[nodiscard]] auto& get_image_data()
    // {
    //     return images;
    // }
    [[nodiscard]] auto& _get_image_data()
    {
        return _images;
    }
    [[nodiscard]] auto& get_buffer_data()
    {
        return _buffers;
    }
    [[nodiscard]] auto& get_as_data()
    {
        return _as;
    }

private:
    void add(std::vector<std::shared_ptr<Image>> data, vk::DescriptorSetLayoutBinding binding_data);
    void add(std::vector<std::shared_ptr<Buffer>> data, vk::DescriptorSetLayoutBinding binding_data);
    void add(std::vector<std::shared_ptr<AccelerationStructure_Top>> data, vk::DescriptorSetLayoutBinding binding_data);
    void update(std::vector<std::shared_ptr<Buffer>> new_data,
                vk::DescriptorSetLayoutBinding binding_data,
                int set_index);

    void update(std::vector<std::shared_ptr<Image>> new_data,
                vk::DescriptorSetLayoutBinding binding_data,
                int set_index);

    void update(std::vector<std::shared_ptr<AccelerationStructure_Top>> new_data,
                vk::DescriptorSetLayoutBinding binding_data,
                int set_index);

    std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;
    vk::DescriptorSetLayout layout;
    // std::shared_ptr<DescriptorPool> pool;

    // data

    // std::vector<Data_Binding<Image>> images;
    // std::vector<Data_Binding<Buffer>> buffers;
    // std::vector<Data_Binding<AccelerationStructure_Top>> as;

    std::unordered_map<uint32_t, Data_Binding<Image>> _images;
    std::unordered_map<int, Data_Binding<Buffer>> _buffers;
    std::unordered_map<int, Data_Binding<AccelerationStructure_Top>> _as;
};
} // namespace MoCheng3D