#ifdef __cplusplus
#include <glm/glm.hpp>
using mat4 = glm::mat4;
#define BEGIN_ENUM(a) enum a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(Graphic_Set)
e_graphic = 0,
    // e_graphic_global = 1,
    graphic_count = 1 END_ENUM();

BEGIN_ENUM(Graphic_Binding)
e_camera_matrix = 0,
    e_textures = 1
    // e_graphic_global = 1,
    END_ENUM();

