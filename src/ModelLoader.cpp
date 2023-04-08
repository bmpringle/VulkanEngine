#include "ResourcePathResolver.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

//#include "Vertex.h"

#include <vector>

Assimp::Importer importer;

void import_test() {
    const aiScene* scene = importer.ReadFile(resolve_resource_path("assets/floor_tile.obj"),
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_EmbedTextures);

    if(scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == AI_SCENE_FLAGS_INCOMPLETE) {
        throw std::runtime_error("error when trying to load model XXXXX");
    }

    if(scene->mNumMeshes != 1) {
        throw std::runtime_error("this loader doesn't support more than one mesh per file, since the engine doesn't support hierarchical meshes.");
    }

    auto mesh = scene->mMeshes[0];
    auto material = scene->mMaterials[mesh->mMaterialIndex];

    //std::vector<Vertex> vertices;

    for(int i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
            
            
    }
}