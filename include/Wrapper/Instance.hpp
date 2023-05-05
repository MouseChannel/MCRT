#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {

class Instance : public Component<vk::Instance, Instance> {

public:
    Instance();

private:
    auto get_required_extension();
    vk::DebugUtilsMessengerEXT m_debugger;
};
} // namespace MCRT