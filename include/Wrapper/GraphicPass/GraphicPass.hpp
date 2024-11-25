#pragma once

#include "Rendering/GraphicContext.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>
#include <vector>
namespace MCRT {
class DrawCall;
class GraphicPass {
public:
    GraphicPass() = delete;
    GraphicPass(GraphicContext* graphic_context);
   
    
  
    void link_renderTarget(std::vector<int> color_renderTargets, std::vector<int> depth_renderTargets, std::vector<int> input_renderTargets, std::vector<int> resolve_renderTargets);
  
    auto get_subpass_index()
    {
        return subpass_index;
    }
    auto get_description()
    {
        return description;
    }
    auto set_subpass_index(int v)
    {
        subpass_index = v;
    }
 

    std::vector<vk::AttachmentReference> color_references;
    std::vector<vk::AttachmentReference> depth_references;
    std::vector<vk::AttachmentReference> input_references;
    std::vector<vk::AttachmentReference> resolve_references;


private:
    std::vector<vk::SubpassDependency> dependencies;
    GraphicContext* m_graphicContext;
    vk::SubpassDescription description;

    int subpass_index = 0;
};
}