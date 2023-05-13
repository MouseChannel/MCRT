#include "Helper/Model_Loader/Obj_Loader.hpp"
#include "Rendering/Model.hpp"
#include "Tool/tiny_obj_loader.hpp"
namespace MCRT {
void Obj_loader::load_model(std::string_view obj_path)
{
    tinyobj::ObjReader reader;
    reader.ParseFromFile(std::string(obj_path));
    auto attrib = reader.GetAttrib();
    auto shapes = reader.GetShapes();
    auto materials = reader.GetMaterials();
    Model::models.clear();
    std::vector<Vertex> vertexs;
    std::vector<uint32_t> indexs;
    for (const auto& shape : shapes) {
        vertexs.clear();
        indexs.clear();
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
                0 }
        };

        Model::models.emplace_back(new Model(shape.name, vertexs, indexs, cur_material));
    }
}
}