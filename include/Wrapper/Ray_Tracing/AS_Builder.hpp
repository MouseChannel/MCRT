#pragma once
#include "Helper/Instance_base.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Mesh;
class Device;
class Buffer;
class AccelerationStructure_Bottom;
class AccelerationStructure_Top;
struct BuildTopAccelerationStructure {
    // hold the instance
    std::shared_ptr<Buffer> instance_buffer;

    vk::AccelerationStructureBuildGeometryInfoKHR build_info;
    vk::AccelerationStructureBuildSizesInfoKHR size_info;
    std::shared_ptr<Buffer> dst_buffer;
    std::vector<vk::AccelerationStructureInstanceKHR> instances;

    // the result of acceleration_structure;
    vk::AccelerationStructureKHR as;
};
class AS_Builder : public Instance_base<AS_Builder> {
public:
    AS_Builder();
    void build_blas();
    vk::DeviceAddress get_blas_device_address(uint32_t blas_index);

    void add_blas_obj(std::shared_ptr<Mesh> obj);

    void build_tlas();
    [[nodiscard("missing tlas")]] auto get_tlas()
    {
        return top_as;
    }

private:
    std::vector<std::shared_ptr<AccelerationStructure_Bottom>> m_accelerate_structures;
    std::shared_ptr<AccelerationStructure_Top> top_as;

    // BuildTopAccelerationStructure build_top_as;
};
}
