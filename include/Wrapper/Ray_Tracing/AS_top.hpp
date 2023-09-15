#pragma once
#include "Wrapper/Ray_Tracing/AS_base.hpp"
#include <vector>
namespace MCRT {
class AccelerationStructure_Bottom;
struct geom_ready_data_top {
    vk::DeviceOrHostAddressConstKHR address;
    vk::AccelerationStructureGeometryInstancesDataKHR instance_geom;
    vk::AccelerationStructureGeometryDataKHR top_as_geom_data;
    vk::AccelerationStructureGeometryKHR top_as_geom;
};

class AccelerationStructure_Top : public AccelerationStructure {
public:
    AccelerationStructure_Top(std::vector<std::shared_ptr<AccelerationStructure_Bottom>> as_bottoms);
    ~AccelerationStructure_Top();
    void add_as_bottom(std::shared_ptr<AccelerationStructure_Bottom> as_bottom);
    void fill_build_info() override;
    // void build();
    void update(std::shared_ptr<Buffer> scratch_buffer) override;

private:
    std::vector<vk::AccelerationStructureInstanceKHR> instances;
    std::shared_ptr<Buffer> instance_buffer;
    geom_ready_data_top ready_data;
    std::vector<std::shared_ptr<AccelerationStructure_Bottom>> bottoms_as;
};
}