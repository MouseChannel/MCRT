#pragma once
#include "Wrapper/Component.hpp"
#include "Wrapper/Ray_Tracing/AS_base.hpp"
namespace MCRT {
class Mesh;
class Buffer;
struct Obj_geom_data {
    vk::AccelerationStructureGeometryKHR as_geom;
    vk::AccelerationStructureBuildRangeInfoKHR offset;
};
struct BuildAccelerationStructure {

    vk::AccelerationStructureBuildGeometryInfoKHR build_info;
    std::vector<vk::AccelerationStructureBuildRangeInfoKHR> range_info;
    vk::AccelerationStructureBuildSizesInfoKHR size_info;
    std::shared_ptr<Buffer> dst_buffer;
};

class AccelerationStructure_Bottom : public AccelerationStructure {
public:
    AccelerationStructure_Bottom(std::shared_ptr<Mesh> obj);
    ~AccelerationStructure_Bottom();
    vk::DeviceAddress get_address();
    auto get_obj_index()
    {
        return obj_index;
    }
    auto get_model_matrix()
    {
        return model_matrix;
    }
    auto get_scratch_size()
    {
        return size_info.buildScratchSize;
    }
    auto get_structure_size()
    {
        return size_info.accelerationStructureSize;
    }
    // void build(std::shared_ptr<Buffer> scratch_buffer);

private:
    // fill geom_ready_data
    void object_to_vkGeometryKHR(std::shared_ptr<Mesh> obj);

    void fill_build_info() override;
    int obj_index;
    vk::TransformMatrixKHR model_matrix;
    Obj_geom_data geom_ready_data;
    // build data
    // vk::AccelerationStructureBuildGeometryInfoKHR build_info;
    // vk::AccelerationStructureBuildRangeInfoKHR range_info;
    // vk::AccelerationStructureBuildSizesInfoKHR size_info;

    // std::shared_ptr<Buffer> dst_buffer;
    // create outside and hold only one overworld
    std::shared_ptr<Buffer> scratch_buffer;
};

}