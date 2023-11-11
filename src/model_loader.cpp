#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>   // Post processing flags
#include "mesh_asset.h"
#include "model_loader.h"

using namespace modelViewer::res;


glm::vec3 to_vec3(const aiVector3D& vec)
{
    return {vec.x,vec.y,vec.z};
}

glm::vec2 to_vec2(const aiVector3D& vec)
{
    return {vec.x,vec.y};
}

mesh_asset model_loader::load(std::filesystem::path filePath) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    auto proccessing =
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType;
    
    const aiScene* scene = importer.ReadFile(filePath.string(), proccessing);

    if (scene == nullptr) 
    {
        throw std::runtime_error(importer.GetErrorString());
    }


    if (!scene->HasMeshes())
    {
        throw std::runtime_error("model in path:" + filePath.string() + " has no mesh defined in it!");
    }

    if (!scene->mMeshes[0]->HasPositions())
    {
        throw std::runtime_error("first mesh in path:" + filePath.string() + " has no positions defined in it!");
    }
    
    auto vertexCount = scene->mMeshes[0]->mNumVertices;
    auto positions = new glm::vec3[vertexCount];
    for (int i = 0; i < vertexCount; ++i)
    {
        auto vertex = scene->mMeshes[0]->mVertices[i];
        positions[i] = to_vec3(vertex);
    }
    
    glm::vec3* normals = nullptr;
    if (scene->mMeshes[0]->HasNormals())
    {
        normals = new glm::vec3[vertexCount];
        for (int i = 0; i < vertexCount; ++i)
        {
            auto normal = scene->mMeshes[0]->mNormals[i];
            normals[i] = to_vec3(normal);
        }
    }

    glm::vec2* uv0 = nullptr;
    if (scene->mMeshes[0]->HasTextureCoords(0))
    {
        uv0 = new glm::vec2[vertexCount];
        for (int i = 0; i < vertexCount; ++i)
        {
            auto uv = scene->mMeshes[0]->mTextureCoords[0][i];
            uv0[i] = to_vec2(uv);
        }
    }
    
    return mesh_asset(vertexCount, positions, normals, uv0);
}
