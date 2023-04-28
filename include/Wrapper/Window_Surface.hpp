#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {
class Surface : public Component<vk::SurfaceKHR, Surface> {
public:
  Surface();
  ~Surface();

private:
};
} // namespace MCRT