#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Rendering/Model.hpp"
#include "Tool/tiny_obj_loader.hpp"
#include "Wrapper/Texture.hpp"
namespace MCRT {
int handle_texture(std::string texture_path)
{
    return -1;

    if (texture_path.empty())
        return -1;

    Texture::textures.emplace_back(new Texture(texture_path));
    return Texture::textures.size() - 1;
}
void Obj_loader::load_model(std::string_view obj_path)
{
    tinyobj::ObjReader reader;
    reader.ParseFromFile(std::string(obj_path));
    auto attrib = reader.GetAttrib();
    auto shapes = reader.GetShapes();
    auto materials = reader.GetMaterials();
    Mesh::all_meshs.clear();
    std::vector<Vertex> vertexs;
    std::vector<uint32_t> indexs;
    std::vector<Triangle> triangles;
    for (const auto& shape : shapes) {
        vertexs.clear();
        indexs.clear();
        triangles.clear();
        for (const auto& index : shape.mesh.indices) {
            vertexs.emplace_back(Vertex {
                .pos {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                },
                .nrm {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                },
            });
            indexs.push_back(indexs.size());
        }
        auto cur = materials[shape.mesh.material_ids[0]];
        // texture

        Material cur_material {
            .color {
                cur.diffuse[0],
                cur.diffuse[1],
                cur.diffuse[2],
                1 },
            .emit {
                cur.emission[0],
                cur.emission[1],
                cur.emission[2],
                0 },
            .reflect = false,
            .color_texture_index = handle_texture(cur.diffuse_texname)

        };
        // if (shape.name == "mocheng.003_mocheng.003" || shape.name == "mocheng_mocheng") {
        //     cur_material.reflect = true;
        // }
        // Texture a("D:/BaiduNetdiskDownload/�?爱卡通女孩儿模型/�?爱卡通女孩儿模型/scene/mat4_c.jpg");
        Mesh::all_meshs.emplace_back(new Mesh(shape.name,
                                              vertexs,
                                              indexs,
                                            //   triangles,
                                              cur_material));
    }
}
}