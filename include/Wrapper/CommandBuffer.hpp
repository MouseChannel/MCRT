#pragma once
#include "Wrapper/Component.hpp"
namespace MCRT {
class CommandBuffer : public Component<vk::CommandBuffer, CommandBuffer> {
public:
  CommandBuffer();
  ~CommandBuffer();
  void Begin(vk::CommandBufferUsageFlags begin_flags);
  void End();
  void Reset();
  void BeginRenderPass(vk::RenderPassBeginInfo renderPassInfo,
                       vk::SubpassContents content);
  void EndRenderPass();

  private:
};
} // namespace MCRT