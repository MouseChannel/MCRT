#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Model;
class Device;
class Buffer;
struct Obj_geom_data {
    std::vector<vk::AccelerationStructureGeometryKHR> as_geom;
    std::vector<vk::AccelerationStructureBuildRangeInfoKHR> offset;
};
struct BuildAccelerationStructure {

    vk::AccelerationStructureBuildGeometryInfoKHR build_info;
    std::vector<vk::AccelerationStructureBuildRangeInfoKHR> range_info;
    vk::AccelerationStructureBuildSizesInfoKHR size_info;
    std::shared_ptr<Buffer> dst_buffer;
    // the result of acceleration_structure;
    vk::AccelerationStructureKHR as;
};
class AS_Builder {
public:
    AS_Builder(std::shared_ptr<Device>);
    Obj_geom_data object_to_vkGeometryKHR(std::shared_ptr<Model> obj);
    void build_blas();
    vk::DeviceAddress get_blas_device_address(uint32_t blas_index);
    void build_tlas();
    void add_blas_obj(Obj_geom_data geom_data)
    {
        blas_ready_data.push_back(geom_data);
    }

private:
    std::vector<Obj_geom_data> blas_ready_data;
    std::vector<BuildAccelerationStructure> build_as;
    std::shared_ptr<Buffer> scratch_buffer;
    std::shared_ptr<Device> m_device;
};
}