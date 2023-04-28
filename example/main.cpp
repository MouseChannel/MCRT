
#include "example/App.cpp"
#include <iostream>

int main(int, char**)
{

    MCRT::App app;
    app.init();
    app.run();
    VkAccelerationStructureCreateInfoKHR a;
    // vkGetAccelerationStructureBuildSizesKHR()

    std::cout
        << "Hello, world!\n";
}
