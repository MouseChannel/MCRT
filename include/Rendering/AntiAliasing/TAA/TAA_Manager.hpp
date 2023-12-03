#include "Helper/Instance_base.hpp"
#include "Helper/Uniform_Manager.hpp"
#include "glm/glm.hpp"
#include "shaders/AntiAliasing/TAA/binding_taa.h"

namespace MCRT {

class ComputePass;
class Image;

class TAA_Manager : public Instance_base<TAA_Manager> {
public:
    void Init(std::shared_ptr<Image> offscreen_image);
    void TAA_Pass(std::shared_ptr<Image>cur_offscreen_image, std::shared_ptr<Image> depth_image,std::shared_ptr<Image> gbuffer_position);

private:
    std::shared_ptr<ComputePass> context;
    std::shared_ptr<Uniform_Stuff<TAA_Data>> taa_data;
    std::shared_ptr<Image> cur_frame_image;
    std::shared_ptr<Image> last_frame_image;
    //    glm::mat4 taa_data;
};
}
