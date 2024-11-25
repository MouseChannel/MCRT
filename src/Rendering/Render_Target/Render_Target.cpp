#include "Rendering/Render_Target/Render_Target.hpp"
 
namespace MCRT {
 
RenderTarget::~RenderTarget()
{
 
    m_image.reset();
}
}