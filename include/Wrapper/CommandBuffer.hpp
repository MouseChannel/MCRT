#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {
class CommandBuffer : public Component<vk::CommandBuffer, CommandBuffer> {
public:
  CommandBuffer();
  ~CommandBuffer();
  void Begin(vk::CommandBufferUsageFlags begin_flags);
  void End();

private:
};
} // namespace MCRT