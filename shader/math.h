#ifdef __cplusplus

#include "glm/glm.hpp"
using mat3 = glm::mat3;
using vec3 = glm::vec3;
#endif
// mat3 getNormalSpace(vec3 normal)
// {
//     vec3 someVec = vec3(1.0, 0.0, 0.0);
//     float dd = dot(someVec, normal);
//     vec3 tangent = vec3(0.0, 1.0, 0.0);
//     if (1.0 - abs(dd) > 1e-6) {
//         tangent = normalize(cross(someVec, normal));
//     }
//     vec3 bitangent = cross(normal, tangent);
//     return mat3(tangent, bitangent, normal);
// }