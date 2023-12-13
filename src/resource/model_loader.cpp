#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>   // Post processing flags
#include "mesh_asset.h"
#include "model_loader.h"
#include "model_info.h"
#include "shader_asset.h"

using namespace modelViewer::res;


auto processSteps =
        aiProcess_CalcTangentSpace       |
        aiProcess_ConvertToLeftHanded    |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType | aiProcess_ImproveCacheLocality;

glm::vec3 to_vec3(const aiVector3D& vec)
{
    return {vec.x,vec.y,vec.z};
}

glm::vec2 to_vec2(const aiVector3D& vec)
{
    return {vec.x,vec.y};
}

texture_asset_wrapping_mode getWrappingMode(aiTextureMapMode mode)
{
    switch (mode) {

        case aiTextureMapMode_Wrap:
            return texture_asset_wrapping_mode::wrap;
        case aiTextureMapMode_Clamp:
            return texture_asset_wrapping_mode::clamp;
        case aiTextureMapMode_Mirror:
            return texture_asset_wrapping_mode::mirror;
        default:
            return texture_asset_wrapping_mode::unsupported;
    }
}

texture_asset_mapping_mode getMappingMode(aiTextureMapping mode)
{
    switch (mode) {
        
        case aiTextureMapping_UV:
            return texture_asset_mapping_mode::uv;
        default:
            return texture_asset_mapping_mode::unsupported;
    }
}

texture_asset_type getTextureType(aiTextureType type)
{
    switch (type) {

        case aiTextureType_DIFFUSE:
            return texture_asset_type::diffuse;
        case aiTextureType_NORMALS:
            return texture_asset_type::normal;
        default:
            return texture_asset_type::none;
    }
}

color toColor(const aiColor3D& c)
{
    return {c.r, c.g, c.b};
}

void model_loader::setMaterialProperties(aiMaterial &material, material_property_set &set) {

    aiColor3D diffuse;
    if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
    {
        set.diffuseAlbedo = toColor (diffuse);
    }


    aiColor3D ambient;
    if (AI_SUCCESS ==  material.Get(AI_MATKEY_COLOR_AMBIENT, ambient))
    {
        set.ambient = toColor (ambient);
    }
    
    aiColor3D spec;
    if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_SPECULAR, spec))
    {
        set.specularAlbedo = toColor (spec);
    }

    float shin;
    if (AI_SUCCESS ==  material.Get(AI_MATKEY_SHININESS, shin))
    {
        set.shininess = shin;
    }

	float opacity;
	if (AI_SUCCESS ==  material.Get(AI_MATKEY_OPACITY, opacity))
	{
		set.opacity = opacity;
	}
    
    //TODO add blending mode
    //TODO add face culling 
}

void model_loader::setShaders(aiMaterial& material, std::vector<shader_asset_info>& shaders)
{
    aiShadingMode model;
    material.Get(AI_MATKEY_SHADING_MODEL, model);
	bool hasNormalMap = material.GetTextureCount(aiTextureType_NORMALS) > 0;
    
    switch (model) {
        
        case aiShadingMode_Gouraud:
            shaders.emplace_back(m_GouraudVertPath, shaderType::vertex);
            shaders.emplace_back(m_GouraudFragPath, shaderType::fragment);
            return;
        default:
			if (hasNormalMap)
			{
				shaders.emplace_back(m_PhongNormalVertPath, shaderType::vertex);
				shaders.emplace_back(m_PhongNormalFragPath, shaderType::fragment);
			}
			else
			{
				shaders.emplace_back(m_PhongVertPath, shaderType::vertex);
				shaders.emplace_back(m_PhongFragPath, shaderType::fragment);
			}
      
            return;
    }
}

void fetchTextures(aiMaterial* material, model_info& info, aiTextureType type)
{
    int texturesCount = material->GetTextureCount(type);
    
    for (int i = 0; i < texturesCount; ++i) {
        
        aiString path;
        aiTextureMapping mapping;
        unsigned uvIndex = 0;
        aiTextureMapMode wrapMode;
        material->GetTexture(type, i ,&path, &mapping,&uvIndex, nullptr, nullptr, &wrapMode);
        
        texture_asset_info textureAssetInfo;
        textureAssetInfo.uvIndex = uvIndex;
        textureAssetInfo.paths.emplace_back(path.C_Str());
        textureAssetInfo.wrappingMode = getWrappingMode(wrapMode);
        textureAssetInfo.mappingMode = getMappingMode(mapping);
        textureAssetInfo.type = getTextureType(type);
        info.material.textures.push_back(textureAssetInfo);
    }
}


std::shared_ptr<std::vector<glm::vec3>> getTangents(const aiMesh& mesh, const aiMatrix4x4 &transform, unsigned int vertexCount)
	{
		if (!mesh.HasTangentsAndBitangents())
		{
			return nullptr;
		}
		
		auto tangents = std::make_shared<std::vector<glm::vec3>>();

		for (int i = 0; i < vertexCount; ++i)
		{
			auto tangent = transform * mesh.mTangents[i];
			tangents->push_back(to_vec3(tangent));
		}

        return tangents;
	}

std::shared_ptr<std::vector<glm::vec3>> getPositions(const aiMesh& mesh,
	const aiMatrix4x4& transform,
	unsigned int vertexCount)
{
	auto positions = std::make_shared<std::vector<glm::vec3>>();
	for (int i = 0; i < vertexCount; ++i)
	{
		auto vertex = transform * mesh.mVertices[i];
		positions->push_back(to_vec3(vertex));
	}
	return positions;
}
std::shared_ptr<std::vector<unsigned int>> getIndices(const aiMesh& mesh)
{
	auto indices = std::make_shared<std::vector<unsigned int>>();
	auto facesCount = mesh.mNumFaces;
	for (int i = 0; i < facesCount; ++i)
	{
		auto face = mesh.mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j) {
			indices->push_back(face.mIndices[j]);
		}
	}
	return indices;
}
std::shared_ptr<std::vector<glm::vec3>> getNormals(const aiMesh& mesh,
	const aiMatrix4x4& transform,
	unsigned int vertexCount)
{
	std::shared_ptr<std::vector<glm::vec3>> normals;
	if (!mesh.HasNormals())
	{
		return normals;
	}
	normals = std::make_shared<std::vector<glm::vec3>>();
	for (int i = 0; i < vertexCount; ++i)
	{
		auto normal = transform * mesh.mNormals[i];
		normals->push_back(to_vec3(normal));
	}
	return normals;
}
std::shared_ptr<std::vector<glm::vec2>> getUV0(const aiMesh& mesh, const aiMatrix4x4& transform, unsigned int vertexCount)
{
	std::shared_ptr<std::vector<glm::vec2>> uv0;
	if (!mesh.HasTextureCoords(0))
	{
		return uv0;
	}
	uv0 = std::make_shared<std::vector<glm::vec2>>();
	for (int i = 0; i < vertexCount; ++i)
	{
		auto uv = transform * mesh.mTextureCoords[0][i];
		uv0->push_back(to_vec2(uv));
	}
	return uv0;
}
std::shared_ptr<mesh_asset> getMesh(const aiScene* scene, std::string& filePath)
{
    if (!scene->HasMeshes())
    {
        throw std::runtime_error("model in path:" + filePath + " has no mesh defined in it!");
    }

    if (!scene->mMeshes[0]->HasPositions())
    {
        throw std::runtime_error("first mesh in path:" + filePath + " has no positions defined in it!");
    }

	auto firstMesh = scene->mMeshes[0];
    auto transform =  scene->mRootNode->mTransformation;
    auto vertexCount = scene->mMeshes[0]->mNumVertices;
	
	std::shared_ptr<std::vector<glm::vec3>> positions = getPositions(*firstMesh, transform, vertexCount);
	std::shared_ptr<std::vector<unsigned int>> indices = getIndices(*firstMesh);
	std::shared_ptr<std::vector<glm::vec3>> normals = getNormals(*firstMesh, transform, vertexCount);
	std::shared_ptr<std::vector<glm::vec2>> uv0 = getUV0(*firstMesh, transform, vertexCount);
	
	std::shared_ptr<std::vector<glm::vec3>> tangents =  getTangents(*firstMesh,transform,vertexCount);
	

	auto mesh = std::make_shared<mesh_asset>();
	mesh->positions = positions;
	mesh->normals = normals;
	mesh->tangents = tangents;
	mesh->indices = indices;
	mesh->UV0 = uv0;
	mesh->name = filePath;
    return mesh;
}

void model_loader::load(std::string filePath, model_info& info)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, processSteps);
    
    if (scene == nullptr) 
    {
        std::cerr << importer.GetErrorString() << std::endl;
        throw std::runtime_error("could not load the file");
    }
    
    if (scene->mNumMaterials > 0)
    {
        auto material = scene->mMaterials[0];
        fetchTextures(material, info, aiTextureType_DIFFUSE);
        fetchTextures(material, info, aiTextureType_NORMALS);
        setShaders(*material, info.material.shaders);
        setMaterialProperties(*material, info.material.propertySet);
    }
    else
    {
        info.material.shaders.emplace_back(m_PhongVertPath, shaderType::vertex);
        info.material.shaders.emplace_back(m_PhongFragPath, shaderType::fragment);
    }
 
    
    info.mesh = getMesh(scene, filePath);
}

std::shared_ptr<mesh_asset> model_loader::loadMesh(std::string filePath) {
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, processSteps);

    if (scene == nullptr)
    {
        std::cerr << importer.GetErrorString() << std::endl;
        throw std::runtime_error("could not load the file");
    }
    
    return getMesh(scene, filePath);
}


