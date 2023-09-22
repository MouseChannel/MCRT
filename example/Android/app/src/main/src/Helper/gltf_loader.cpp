// #include "Rendering/Model.hpp"
// #include "Wrapper/Texture.hpp"
#include <iostream>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "Rendering/Context.hpp"
#include "Rendering/Model.hpp"

#include <glm/gtc/type_ptr.hpp>

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#define TINYGLTF_IMPLEMENTATION
#endif
#include "Helper/gltf_loader.hpp"
namespace MoCheng3D {
using Vertex = Model::Vertex;
int get_size(int componentType)
{
    switch (componentType) {
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:

    case TINYGLTF_PARAMETER_TYPE_FLOAT:
        return 4;

    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
        return 2;

    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
        return 1;
    default:
        throw std::runtime_error("unsupport type");
    }
}
static std::string GetFilePathExtension(const std::string& FileName)
{
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}
glm::mat4 translation_to_matrix(const std::vector<double>& translation)
{
    double x = translation[0];
    double y = translation[1];
    double z = translation[2];

    glm::mat4 mat(1);
    // mat[0][3] = x;
    // mat[1][3] = y;
    // mat[2][3] = z;
    mat = glm::translate(mat, glm::vec3 { x, y, z });

    return mat;
}
glm::mat4 scale_to_matrix(const std ::vector<double>& scale)
{
    double x = scale[0];
    double y = scale[1];
    double z = scale[2];

    glm::mat4 mat(1);
    // mat[0][0] = x;
    // mat[1][1] = y;
    // mat[2][2] = z;
    mat = glm::scale(mat, { x, y, z });

    return mat;
}
glm::mat4 quaternion_to_matrix(std ::vector<double> quaternion)
{
    auto x = quaternion[0];
    auto y = quaternion[1];
    auto z = quaternion[2];
    auto w = quaternion[3];

    float x2 = x * 2;
    float y2 = y * 2;
    float z2 = z * 2;
    float wx = x2 * w;
    float wy = y2 * w;
    float wz = z2 * w;
    float xx = x2 * x;
    float xy = y2 * x;
    float xz = z2 * x;
    float yy = y2 * y;
    float yz = z2 * y;
    float zz = z2 * z;
    glm::mat4 mat;
    // mat[0][0] = 1 - (yy + zz);
    // mat[0][1] = xy - wz;
    // mat[0][2] = xz + wy;
    // mat[0][3] = 0.0f;
    // mat[1][0] = xy + wz;
    // mat[1][1] = 1 - (xx + zz);
    // mat[1][2] = yz - wx;
    // mat[1][3] = 0.0f;
    // mat[2][0] = xz - wy;
    // mat[2][1] = yz + wx;
    // mat[2][2] = 1 - (xx + yy);
    // mat[2][3] = 0.0f;
    // mat[3][0] = 0.0f;
    // mat[3][1] = 0.0f;
    // mat[3][2] = 0.0f;
    // mat[3][3] = 1.0f;
    //
    mat[0][0] = 1 - (yy + zz);
    mat[1][0] = xy - wz;
    mat[2][0] = xz + wy;
    mat[3][0] = 0.0f;
    mat[0][1] = xy + wz;
    mat[1][1] = 1 - (xx + zz);
    mat[2][1] = yz - wx;
    mat[3][1] = 0.0f;
    mat[0][2] = xz - wy;
    mat[1][2] = yz + wx;
    mat[2][2] = 1 - (xx + yy);
    mat[3][2] = 0.0f;
    mat[0][3] = 0.0f;
    mat[1][3] = 0.0f;
    mat[2][3] = 0.0f;
    mat[3][3] = 1.0f;
    // mat = glm::transpose(mat);
    return mat;
}

template <typename T>
void copy_data(std::vector<T>& data, const std::vector<unsigned char>& src, tinygltf::BufferView buffer_view, int count)
{
    data.resize(count);

    std::memcpy(data.data(), src.data() + buffer_view.byteOffset, count * sizeof(T));
}

// int handle_texture(tinygltf::Model model, tinygltf::TextureInfo texture_info)
// {

//     if (texture_info.index < 0)
//         return -1;

//     auto texture = model.textures[texture_info.index];
//     if (texture.source < 0)
//         return -1;
//     auto image = model.images[texture.source];
//     // texture
//     if (image.image.size() < 0)
//         return -1;
//     texture_lock.lock();
//     Texture::textures.emplace_back(new Texture(image.image.data(), image.width, image.height, image.image.size()));
//     int index = Texture::textures.size() - 1;
//     texture_lock.unlock();
//     return index;
// }
// int handle_texture(tinygltf::Model model, tinygltf::NormalTextureInfo texture_info)
// {

//     if (texture_info.index < 0)
//         return -1;

//     auto texture = model.textures[texture_info.index];
//     if (texture.source < 0)
//         return -1;
//     auto image = model.images[texture.source];
//     // texture
//     if (image.image.size() < 0)
//         return -1;
//     texture_lock.lock();
//     Texture::textures.emplace_back(new Texture(image.image.data(), image.width, image.height, image.image.size()));
//     int index = Texture::textures.size() - 1;
//     texture_lock.unlock();
//     return index;
// }
// std::shared_ptr<Mesh> GLTF_Loader::load_skybox(std::string_view path)
// {
//     load_model(path);
//     auto sky_box = Mesh::meshs[Mesh::meshs.size() - 1];
//     Mesh::meshs.pop_back();
//     return sky_box;
// }
glm::mat4 GLTF_Loader::load_primitive(glm::mat4 father_matrix,
    const tinygltf::Model& model,
    const tinygltf::Node& node,
    tinygltf::Primitive& primitive)
{
    auto get_accessor = [&](int index) {
        return model.accessors[index];
    };
    auto get_buffer_view = [&](int index) {
        auto accessor = get_accessor(index);
        return model.bufferViews[accessor.bufferView];
    };
    std::vector<Vertex> vertexs;
    std::vector<uint32_t> indices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoord;

    std::vector<unsigned short> indexs;
    // node_number += node.children.size() + 1;
    // std::cout << ++node_number << "/" << model.scenes[model.defaultScene].nodes.size() << "    loading: " << node.name << std::endl;

    glm::mat4 translation_matrix(1);
    if (!node.translation.empty()) {
        translation_matrix = translation_to_matrix(node.translation);
    }
    glm::mat4 scale_matrix(1);
    if (!node.scale.empty()) {
        scale_matrix = scale_to_matrix(node.scale);
    }
    glm::mat4 rotate_matrix(1);
    if (!node.rotation.empty()) {
        rotate_matrix = quaternion_to_matrix(node.rotation);
    }

    glm::mat4 local_matrix = translation_matrix * rotate_matrix * scale_matrix;
    local_matrix = father_matrix * local_matrix;
    std::array<std::array<float, 4>, 3> transform;
    {
        transform[0] = {
            local_matrix[0][0],
            local_matrix[1][0],
            local_matrix[2][0],
            local_matrix[3][0]
        };
        transform[1] = {
            local_matrix[0][1],
            local_matrix[1][1],
            local_matrix[2][1],
            local_matrix[3][1]
        };
        transform[2] = {
            local_matrix[0][2],
            local_matrix[1][2],
            local_matrix[2][2],
            local_matrix[3][2]
        };
    }
    if (primitive.mode != 4) {
        throw std::runtime_error("not triangle");
    }
    { // indices
        indexs.clear();
        auto indices_index = primitive.indices;
        auto accessor_indices = get_accessor(indices_index);
        auto buffer_view = get_buffer_view(indices_index);

        copy_data(indexs, model.buffers[buffer_view.buffer].data, buffer_view, accessor_indices.count);
    }
    { // position
        positions.clear();
        auto position_index = primitive.attributes["POSITION"];
        auto accessor_position = get_accessor(position_index);

        auto buffer_view = get_buffer_view(position_index);

        if (accessor_position.type != TINYGLTF_TYPE_VEC3) {
            throw std::runtime_error("position format is not vec3");
        }
        copy_data(positions, model.buffers[buffer_view.buffer].data, buffer_view, accessor_position.count);
    }
    { // normal
        normals.clear();
        auto normal_index = primitive.attributes["NORMAL"];
        auto accessor_normal = get_accessor(normal_index);

        auto buffer_view = get_buffer_view(normal_index);

        if (accessor_normal.type != TINYGLTF_TYPE_VEC3) {
            throw std::runtime_error("normal format is not vec3");
        }
        copy_data(normals, model.buffers[buffer_view.buffer].data, buffer_view, accessor_normal.count);
    }
    { // texcoord
        texcoord.clear();
        if (primitive.attributes.contains("TEXCOORD_0")) {

            auto texcoord_index = primitive.attributes["TEXCOORD_0"];
            auto accessor_texcoord = get_accessor(texcoord_index);

            auto buffer_view = get_buffer_view(texcoord_index);

            if (accessor_texcoord.type != TINYGLTF_TYPE_VEC2) {
                throw std::runtime_error("texcoord format is not vec2");
                // std::cout << "texcoord not vec3" << std::endl;
                return local_matrix;
            }
            copy_data(texcoord, model.buffers[buffer_view.buffer].data, buffer_view, accessor_texcoord.count);
        }
    }
    vertexs.clear();
    indices.clear();

    std::array<glm::vec3, 3> triangle_pos;
    for (int i = 0; i < indexs.size(); i += 3) {

        for (int j = 0; j < 3; j++) {
            vertexs.emplace_back(Vertex {
                .pos = positions[indexs[i + j]],
                .nrm = normals[indexs[i + j]],
                .texCoord = texcoord.empty() ? glm::vec2 { 0 } : texcoord[indexs[i + j]] });
            indices.push_back(indices.size());
            triangle_pos[j] = positions[indexs[i + j]];
        }
    }

    Model::meshs.emplace_back(new Model(vertexs, indices));
    // Model::meshs.emplace_back(new Mesh(node.name,
    //     vertexs,
    //     indices,
    //     triangles,
    //     cur_material,
    //     transform));
    return local_matrix;
}
void GLTF_Loader::load_mesh(glm::mat4 local_matrix, const tinygltf::Model& model, const tinygltf::Node& node)
{
    glm::mat4 cur_marix { 1 };
    if (node.mesh >= 0) {

        auto mesh = model.meshes[node.mesh];

        for (auto& primitive : mesh.primitives) {
            cur_marix = load_primitive(local_matrix, model, node, primitive);
        }
    }
    for (auto& child : node.children) {
        load_mesh(cur_marix, model, model.nodes[child]);
    }
}

void GLTF_Loader::load_model(std::string_view path)
{

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool ret = false;

#if defined(VK_USE_PLATFORM_ANDROID_KHR)

    tinygltf::asset_manager = MoCheng3D::Context::Get_Singleton()->Get_app()->activity->assetManager;
#endif

    auto file_extension = GetFilePathExtension(std::string(path));
    if (file_extension == "glb")
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, std::string(path)); // for binary glTF(.glb)
    else if (file_extension == "gltf")
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, std::string(path));
    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        throw std::runtime_error("Failed to parse glTF\n");
    }

    int node_number = 0;

    std::cout << "loading Model" << std::endl;

    for (auto& node_index : model.scenes[model.defaultScene].nodes) {
        auto node = model.nodes[node_index];
        std::cout << ++node_number << "/" << model.scenes[model.defaultScene].nodes.size() << "    loading: " << node.name << std::endl;

        load_mesh(glm::mat4 { 1 }, model, node);
    }
}
}