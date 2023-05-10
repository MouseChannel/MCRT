#pragma once
#include "Wrapper/Component.hpp"
#include "Wrapper/Device.hpp"
#include "Wrapper/Instance.hpp"
#include <vulkan/vulkan.hpp>

namespace MCRT {
class Debugger {
public:
 
    template <typename T>
    void set_name(T obj, const std::string name)
    {
        vk::DebugUtilsObjectNameInfoEXT info;
        auto native = obj->get_c_type();
        auto type = obj->get_handle().objectType;
        info.setObjectHandle((uint64_t)native)
            .setPObjectName(name.c_str())
            .setObjectType(type);
        vkSetDebugUtilsObjectNameEXT(Context::Get_Singleton()->get_device()->get_handle(),
                                     (VkDebugUtilsObjectNameInfoEXT*)&info);
    }

private:
    void set_object_name(uint64_t object, std::string name, vk::ObjectType type);
};
}