#pragma once
#include "Tool/tiny_gltf.h"

namespace MCRT {
class GLTF_Loader {
public:
    static void load_model(std::string_view path);
};
}