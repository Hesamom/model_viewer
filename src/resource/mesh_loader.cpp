#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>   // Post processing flags
#include "mesh_asset.h"
#include "mesh_loader.h"

using namespace modelViewer::res;


glm::vec3 to_vec3(const aiVector3D& vec)
{
    return {vec.x,vec.y,vec.z};
}

glm::vec2 to_vec2(const aiVector3D& vec)
{
    return {vec.x,vec.y};
}

std::shared_ptr<mesh_asset> mesh_loader::load(std::filesystem::path filePath) {
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
    auto positions = std::make_shared<std::vector<glm::vec3>>();
    for (int i = 0; i < vertexCount; ++i)
    {
        auto vertex = scene->mMeshes[0]->mVertices[i];
        positions->push_back(to_vec3(vertex));
    }

    auto facesCount = scene->mMeshes[0]->mNumFaces;
    auto indices = std::make_shared<std::vector<unsigned int>>();
    for (int i = 0; i < facesCount; ++i)
    {
        auto face = scene->mMeshes[0]->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j) {
            indices->push_back(face.mIndices[j]);
        }
    }
    
    std::shared_ptr<std::vector<glm::vec3>> normals;
    if (scene->mMeshes[0]->HasNormals())
    {
        normals = std::make_shared<std::vector<glm::vec3>>();
        for (int i = 0; i < vertexCount; ++i)
        {
            auto normal = scene->mMeshes[0]->mNormals[i];
            normals->push_back(to_vec3(normal));
        }
    }

    std::shared_ptr<std::vector<glm::vec2>> uv0;
    if (scene->mMeshes[0]->HasTextureCoords(0))
    {
        uv0 = std::make_shared<std::vector<glm::vec2>>();
        for (int i = 0; i < vertexCount; ++i)
        {
            auto uv = scene->mMeshes[0]->mTextureCoords[0][i];
            uv0->push_back(to_vec2(uv));
        }
    }
    
    importer.FreeScene();
    auto mesh = std::make_shared<mesh_asset>();
    mesh->positions = positions;
    mesh->normals = normals;
    mesh->indices = indices;
    mesh->UV0 = uv0;
    mesh->name = filePath.string();
    
    return mesh;
}
