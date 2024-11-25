#pragma once
#include "Tool/tiny_gltf.h"
#include "glm/glm.hpp"
#include <memory>
namespace MCRT {
class Mesh;
class GLTF_Loader {
public:
    static void load_model(std::string_view path);
    static std::shared_ptr<Mesh> load_skybox(std::string_view path = "/home/mocheng/project/MCRT/assets/cube.gltf");

private:
    static void load_mesh(glm::mat4 local_matrix, const tinygltf::Model& model, const tinygltf::Node& node);
    static glm::mat4 load_primitive(glm::mat4 father_matrix, const tinygltf::Model& model, const tinygltf::Node& node, tinygltf::Primitive& primitive);
};
}