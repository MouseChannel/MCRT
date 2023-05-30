#pragma once

#include <string_view>
namespace MCRT {

class Obj_loader {
public:
    static void load_model(std::string_view path);
};

}