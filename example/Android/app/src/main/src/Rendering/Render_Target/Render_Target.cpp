#include "Rendering/Render_Target/Render_Target.hpp"

// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
namespace MoCheng3D {
RenderTarget::RenderTarget(std::shared_ptr<Image> image,
    vk::AttachmentDescription description)
    : image { image }
    , attachment_description(description)
{
}
RenderTarget::~RenderTarget()
{
    image.reset();
}
}