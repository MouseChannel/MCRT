#include "Rendering/Model.hpp"
#include "Wrapper/Texture.hpp"
#include <filesystem>
#define TINYOBJLOADER_IMPLEMENTATION
#include "Tool/stb_image.h"
#include "Tool/tiny_obj_loader.hpp"
#include "Wrapper/Buffer.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


#include "Helper/Debugger.hpp"
namespace MCRT {
std::vector<ObjInstance> Mesh::obj_instances;
std::vector<std::shared_ptr<Mesh>> Mesh::all_meshs;
std::vector<Material> Mesh::materials;

Mesh::Mesh(std::string name,
           std::vector<Vertex>& vertexs,
           std::vector<uint32_t>& indexs,
        //    std::vector<Triangle>& triangles,
           Material material,
           std::array<std::array<float, 4>, 3> transform)
    : m_name(name)
    , m_vertexs(std::move(vertexs))
    , m_index(std::move(indexs))
    // , triangles(std::move(triangles))

    , m_material(material)
    , tramsform(transform)

{
    assert(m_vertexs.size() > 0);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    auto extra_flag = (vk::BufferUsageFlagBits)0;
#else
    auto extra_flag = (vk::BufferUsageFlags)0;

//        vk::BufferUsageFlagBits::eShaderDeviceAddress |
//        vk::BufferUsageFlagBits::eStorageBuffer |
//        vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
#endif
    vertexs_buffer = Buffer::CreateDeviceBuffer(
        m_vertexs.data(),
        m_vertexs.size() * sizeof(m_vertexs[0]),
        vk::BufferUsageFlagBits::eVertexBuffer | extra_flag);
    indices_buffer = Buffer::CreateDeviceBuffer(
        m_index.data(),
        m_index.size() * sizeof(m_index[0]),
        vk::BufferUsageFlagBits::eIndexBuffer | extra_flag);
    material_buffer = Buffer::CreateDeviceBuffer(&m_material, sizeof(m_material), extra_flag);
    std::cout<< "buffer:  " <<vertexs_buffer->get_handle()<<std::endl;
}

Mesh::Mesh(aiMesh* mesh)
{
    m_vertexs.resize(mesh->mNumVertices);
    for (int i = 0; i < mesh->mNumVertices; i++) {
        m_vertexs[i] = Vertex {
            .pos { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
            .nrm { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z },
            // .color {},
            .texCoord { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y },
            .tangent { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z },
            .bitangent { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z }
        };
    }
    m_index.resize(mesh->mNumFaces * 3);
    for (int i = 0; i < mesh->mNumFaces; i++) {
        for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
            // auto rr = mesh->mFaces[i].mIndices;
            // auto r = rr[0];
            m_index[i * 3 + j] = mesh->mFaces[i].mIndices[j];
        }
    }
    Vertex vertex {};
    vertexs_buffer = Buffer::CreateDeviceBuffer(
        m_vertexs.data(),
        m_vertexs.size() * sizeof(m_vertexs[0]),
        vk::BufferUsageFlagBits::eVertexBuffer);
    indices_buffer = Buffer::CreateDeviceBuffer(
        m_index.data(),
        m_index.size() * sizeof(m_index[0]),
        vk::BufferUsageFlagBits::eIndexBuffer);
    m_material = materials[mesh->mMaterialIndex];
    // std::cout<< "buffer:  " <<vertexs_buffer->get_handle()<<std::endl;
    Context::Get_Singleton()->get_debugger()->set_name(vertexs_buffer,"vertex buffer");
}

int HandleImage(std::string path, vk::Format format)
{
    int width, height, channels;
    auto pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (pixels) {

        Texture::textures.emplace_back(new Texture(pixels, width, height, width * height * 4, format));
    } else {
        throw std::runtime_error("fail read img");
    }
    return Texture::textures.size() - 1;
}
std::shared_ptr<Mesh> Mesh::LoadFromFile(std::string path)
{
    std::shared_ptr<Mesh> mesh;
    Assimp::Importer importer;
    const unsigned int ImportFlags =
        aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType |
        aiProcess_PreTransformVertices | aiProcess_GenNormals |
        aiProcess_GenUVCoords | aiProcess_OptimizeMeshes | aiProcess_Debone |
        aiProcess_ValidateDataStructure;
    const aiScene* scene = importer.ReadFile(path, ImportFlags);
    if (scene && scene->HasMeshes()) {
        materials.resize(scene->mNumMaterials);
        for (int i = 0; i < scene->mNumMaterials; i++) {
            aiString diffName, armName, normalName;
            scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &diffName);
            scene->mMaterials[i]->GetTexture(aiTextureType_METALNESS, 0, &armName);
            scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &normalName);
            // armName = "arm.png";
            // auto diff_texture_index = HandleImage("/home/mousechannel/project/PBR/data/textures/cerberus_A.png", vk::Format ::eR8G8B8A8Srgb);
            // materials[i].color_texture_index = diff_texture_index;
            // auto arm_texture_index = HandleImage("/home/mousechannel/project/PBR/data/textures/arm.png",vk::Format ::eR8G8B8A8Unorm);
            // materials[i].metallicness_roughness_texture_index = arm_texture_index;
            // auto normal_texture_index = HandleImage("/home/mousechannel/project/PBR/data/textures/cerberus_N.png",vk::Format ::eR8G8B8A8Unorm);
            // materials[i].normal_texture_index = normal_texture_index;
            // break;
            auto dir_name = std::filesystem::path(path).parent_path();
            if (diffName.length > 0) {
                auto texture_name = dir_name / std::filesystem::path(diffName.C_Str());
                auto diff_texture_index = HandleImage(texture_name,   vk::Format ::eR8G8B8A8Srgb);
                materials[i].color_texture_index = diff_texture_index;
            }
            if (armName.length > 0) {
                auto texture_name = dir_name / std::filesystem::path(armName.C_Str());
                auto arm_texture_index = HandleImage(texture_name,vk::Format ::eR8G8B8A8Unorm);
                materials[i].metallicness_roughness_texture_index = arm_texture_index;
            }
            if (normalName.length > 0) {

                auto texture_name = dir_name / std::filesystem::path(normalName.C_Str());
                auto normal_texture_index = HandleImage(texture_name,vk::Format ::eR8G8B8A8Unorm);
                materials[i].normal_texture_index = normal_texture_index;
            }
            // materials[i] = Material {
            //     .color_texture_index = diff_texture_index,
            //     .normal_texture_index = normal_texture_index,
            //     .metallicness_roughness_texture_index = arm_texture_index
            // };
        }
        auto& rr =  Texture::textures;
        for (int i = 0; i < scene->mNumMeshes; i++) {
            Mesh::all_meshs.emplace_back(new Mesh { scene->mMeshes[0] });
        }
        int t = 0;
    } else {
        throw std::runtime_error("Failed to load mesh file: " + path);
    }
    // std::filesystem::path specific_path = path;
    // auto p = specific_path.parent_path();
    // scene->mMaterials[0]->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, unsigned int index, aiString *path)
    // scene->mTextures[0]->mFilename;

    // auto r = scene->GetEmbeddedTexture(scene->mTextures[0]->mFilename.C_Str());

    // std::shared_ptr<Texture> te{new Texture()}
    auto& temo = Mesh::all_meshs;
    aiString temp;
    scene->mMaterials[0]->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &temp);
    scene->mMaterials[0]->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &temp);
    scene->mMaterials[0]->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &temp);

    return mesh;
}
void Mesh::update_accelerate_structure_data()
{
    for (int i = 0; i < all_meshs.size(); i++) {
        all_meshs[i]->set_index(i);
    }
    glm::mat4x3 dd;
    obj_instances.clear();
    for (auto i { 0 }; i < all_meshs.size(); i++) {
        obj_instances.emplace_back(ObjInstance {
            .obj_index = i,

            .matrix { vk::TransformMatrixKHR {}.setMatrix(all_meshs[i]->tramsform) } });
    }
}

} // namespace MCRT