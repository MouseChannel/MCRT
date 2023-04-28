#pragma once
#include <memory>
template <typename T>
class Instance_base {
public:
    static std::unique_ptr<T> _instance;
    static std::unique_ptr<T>& Get_Singleton()
    {
        if (!_instance) {
            _instance.reset(new T);
        }
        return  _instance;
    }
     
};
template <typename T>
std::unique_ptr<T> Instance_base<T>::_instance = nullptr;