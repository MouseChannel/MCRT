#pragma once

#include "vulkan/vulkan.hpp"
#include <functional>
#include<memory>
namespace MCRT {
class CommandBuffer;
class CommandManager final {
public:
    using RecordCmdFunc = std::function<void(vk::CommandBuffer& handle)>;
    static void ExecuteCmd(vk::Queue, RecordCmdFunc);
    static void submit_Cmd(vk::Queue, std::shared_ptr<CommandBuffer>);
};

}