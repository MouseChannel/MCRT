#pragma once
#include "Helper/Model_Loader/HDRI2PNG.hpp"
#include <string_view>
namespace MCRT {

class Obj_loader {
public:
    static void load_model(std::string_view path);
};

}