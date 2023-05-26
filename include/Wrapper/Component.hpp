#pragma once
#include "Rendering/Context.hpp"
#include "vulkan/vulkan.hpp"
#include <iostream>
#include <memory>
// #includ

namespace MCRT {

template <typename T, typename K>
class Component {

protected:
    T m_handle { VK_NULL_HANDLE };

    [[nodiscard]] auto& Get_Context_Singleton()
    {
        return Context::Get_Singleton();
    }

public:
    [[nodiscard]]virtual T& get_handle()
    {
        // assert(m_handle);
        return m_handle;
    }
    [[nodiscard]] auto get_c_type()
    {
        return static_cast<typename T::NativeType>(m_handle);
    }
    ~Component()
    {
        // std::cout << typeid(K).name() << std::endl;
    }
};
} // namespace MCRT