#include "Rendering/Render_Target/Render_Target.hpp"

// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
namespace MCRT {
// RenderTarget::RenderTarget(std::shared_ptr<Image> image, int type, vk::AttachmentDescription description)
//     : image { image }
//     , type(type)
//     , attachment_description(description)
// {
// }
RenderTarget::~RenderTarget()
{
 
    m_image.reset();
}
}