#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "App.hpp"
#include <iostream>
#include <vulkan/vulkan_core.h>

int main(int, char**)
{

    MCRT::App app;
    app.init();
    app.run();
     
    return 0;
}
