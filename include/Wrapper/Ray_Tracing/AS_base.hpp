#pragma once
#include "Wrapper/Component.hpp"

namespace MCRT {
class Buffer;
class AccelerationStructure
    : public Component<vk::AccelerationStructureKHR, AccelerationStructure> {
public:
    AccelerationStructure(vk::AccelerationStructureTypeKHR type)
        : m_type(type)
    {
    }
    // auto get_address();

    auto get_scratch_size()
    {
        return size_info.accelerationStructureSize;
    }
    // auto get_structure_size();
    virtual void fill_build_info() = 0;
    void build(std::shared_ptr<Buffer> scratch_buffer);

protected:
    vk::AccelerationStructureTypeKHR m_type;
    // buffer hold this struct
    std::shared_ptr<Buffer> dst_buffer;

    vk::AccelerationStructureBuildGeometryInfoKHR build_info;
    vk::AccelerationStructureBuildRangeInfoKHR range_info;
    vk::AccelerationStructureBuildSizesInfoKHR size_info;
};
}