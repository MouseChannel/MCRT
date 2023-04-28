#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {
class CommandPool : public Component<vk::CommandPool, CommandPool> {
public:
  CommandPool();
  ~CommandPool();

private:
};
} // namespace MCRT