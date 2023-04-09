#include "ModelLoader.h"

#include "ResourcePathResolver.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

Assimp::Importer importer;

std::pair<std::vector<Vertex>, std::string> import_model(std::string relative_path_to_resources, std::string model_file) {
    const aiScene* scene = importer.ReadFile(resolve_resource_path(relative_path_to_resources + model_file),
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipWindingOrder);

    if(scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == AI_SCENE_FLAGS_INCOMPLETE) {
        throw std::runtime_error("error when trying to load model XXXXX");
    }

    if(scene->mNumMeshes != 1) {
        throw std::runtime_error("this loader doesn't support more than one mesh per file, since the engine doesn't support hierarchical meshes.");
    }

    auto mesh = scene->mMeshes[0];

    std::vector<Vertex> vertices;

    for(int i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];

        if(face.mNumIndices != 3) {
            throw std::runtime_error("this model has a face that failed to be properly triangulated");
        }
            
        auto vertex1 = mesh->mVertices[face.mIndices[0]];
        auto vertex2 = mesh->mVertices[face.mIndices[1]];
        auto vertex3 = mesh->mVertices[face.mIndices[2]];

        auto uv1 = mesh->mTextureCoords[0][face.mIndices[0]];
        auto uv2 = mesh->mTextureCoords[0][face.mIndices[1]];
        auto uv3 = mesh->mTextureCoords[0][face.mIndices[2]];

        vertices.push_back({{vertex1.x, vertex1.y, vertex1.z}, {1, 1, 1}, {uv1.x, uv1.y, 0}});
        vertices.push_back({{vertex2.x, vertex2.y, vertex2.z}, {1, 1, 1}, {uv2.x, uv2.y, 0}});
        vertices.push_back({{vertex3.x, vertex3.y, vertex3.z}, {1, 1, 1}, {uv3.x, uv3.y, 0}});
    }

    auto material = scene->mMaterials[mesh->mMaterialIndex];

    aiString texture_name;
    auto ret = material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_name);

    if(ret != aiReturn_SUCCESS) {
        throw std::runtime_error("failure to get texture in slot 0");
    }

    return {vertices, relative_path_to_resources + std::string(texture_name.C_Str())};
}