#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>   // Post processing flags
#include "mesh_asset.h"
#include "model_loader.h"
#include "model_info.h"
#include "shader_asset.h"
#include "texture_asset.h"
#include "../common/stopwatch.h"
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

using namespace modelViewer::res;

const std::string  m_PhongVertPath = "res/shaders/sample/phong_phong_vert.glsl";
const std::string  m_PhongFragPath = "res/shaders/sample/phong_phong_frag.glsl";

const std::string  m_PhongNormalVertPath = "res/shaders/sample/phong_phong_normal_map_vert.glsl";
const std::string  m_PhongNormalFragPath = "res/shaders/sample/phong_phong_normal_map_frag.glsl";

const std::string  m_GouraudVertPath = "res/shaders/sample/phong_gouraud_vert.glsl";
const std::string  m_GouraudFragPath = "res/shaders/sample/phong_gouraud_frag.glsl";


struct model_load_context
{
	const aiScene* scene = nullptr;
	model_info* modelInfo = nullptr;
	std::shared_ptr<material_asset> defaultMaterialAsset;
};

auto processSteps =
        aiProcess_CalcTangentSpace       |
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

std::string getSamplerName(aiTextureType type)
{
    switch (type) {

        case aiTextureType_DIFFUSE:
            return "u_diffuseSampler";
        case aiTextureType_NORMALS:
             return "u_normalSampler";
    	default:
            return {};
    }
}

color toColor(const aiColor3D& c)
{
    return {c.r, c.g, c.b};
}

void setMaterialProperties(aiMaterial &material, material_property_set &set) {

    aiColor3D diffuse;
    if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
    {
		set.colors.push_back({Literals::DiffuseAlbedo, toColor (diffuse)});
    }
	else
	{
		set.colors.push_back({Literals::DiffuseAlbedo, Literals::DefaultDiffuseAlbedo});
	}


    aiColor3D ambient;
    if (AI_SUCCESS ==  material.Get(AI_MATKEY_COLOR_AMBIENT, ambient))
    {
		set.colors.push_back({Literals::AmbientAlbedo, toColor (ambient)});
    }
	else
	{
		set.colors.push_back({Literals::AmbientAlbedo, Literals::DefaultAmbientAlbedo});
	}
    
    aiColor3D spec;
    if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_SPECULAR, spec))
    {
		set.colors.push_back({Literals::SpecularAlbedo, toColor (spec)});
    }
	else
	{
		set.colors.push_back({Literals::AmbientAlbedo, Literals::DefaultSpecularAlbedo});
	}

    float shin;
    if (AI_SUCCESS ==  material.Get(AI_MATKEY_SHININESS, shin))
    {
        set.floats.push_back({Literals::Shininess, shin});
    }
	else
	{
		set.floats.push_back({Literals::Shininess, Literals::DefaultShininess});
	}

	float opacity;
	if (AI_SUCCESS ==  material.Get(AI_MATKEY_OPACITY, opacity))
	{
		set.floats.push_back({Literals::Opacity, opacity});
	}
	else
	{
		set.floats.push_back({Literals::Opacity, Literals::DefaultOpacity});
	}
    
    //TODO add blending mode
    //TODO add face culling 
}

void setShaders(aiMaterial& material, std::vector<shader_asset_info>& shaders)
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
    }
}

std::shared_ptr<texture_embedded> getEmbeddedTexture(const aiScene& scene, const aiString& path) {
	
	const auto embeddedTexture = scene.GetEmbeddedTexture(path.C_Str()) ;
	if (embeddedTexture == nullptr) {
		return nullptr;
	}

	const bool isCompressed = embeddedTexture->mHeight == 0;
	auto textureSize = embeddedTexture->mWidth * embeddedTexture->mHeight;
	constexpr int channelsCount = 4;
	
	if (!isCompressed)
	{
		textureSize *= channelsCount;
	}
	else {
		textureSize = embeddedTexture->mWidth;
	}
	
	auto pixels = new unsigned char[textureSize];

	if(isCompressed) {
		int index = 0;
		for (int i = 0; i < textureSize; i+=4)
		{
			const auto pixel = embeddedTexture->pcData[index];
			pixels[i] = pixel.b;
			pixels[i + 1] = pixel.g;
			pixels[i + 2] = pixel.r;
			pixels[i + 3] = pixel.a;

			index++;
		}
	}
	else {
		int index = 0;
		for (int i = 0; i < textureSize; i+=4)
		{
			const auto pixel = embeddedTexture->pcData[index];
			pixels[i] = pixel.r;
			pixels[i + 1] = pixel.g;
			pixels[i + 2] = pixel.b;
			pixels[i + 3] = pixel.a;

			index++;
		}
	}
	
	
	
	auto asset = std::make_shared<texture_embedded>(pixels, textureSize, 4, embeddedTexture->mWidth , embeddedTexture->mHeight,isCompressed);
	return asset;
}

void fetchTextures(const aiMaterial& material, const aiScene& scene, material_asset& materialInfo, model_info& modelInfo, aiTextureType type)
{
    const unsigned int texturesCount = material.GetTextureCount(type);
    
    for (int i = 0; i < texturesCount; ++i) {
        
        aiString path;
        aiTextureMapping mapping;
        unsigned uvIndex = 0;
        aiTextureMapMode wrapMode;
        material.GetTexture(type, i ,&path, &mapping,&uvIndex, nullptr, nullptr, &wrapMode);
    	
    	std::string fullPath = std::filesystem::path{modelInfo.path}.parent_path().append(std::string(path.C_Str())).string();
        texture_asset_info textureAssetInfo;
        textureAssetInfo.uvIndex = uvIndex;
        textureAssetInfo.paths.emplace_back(fullPath);
        textureAssetInfo.wrappingMode = getWrappingMode(wrapMode);
        textureAssetInfo.mappingMode = getMappingMode(mapping);
        textureAssetInfo.type = texture_asset_type::texture2D;
    	textureAssetInfo.samplerName = getSamplerName(type);

    	auto embeddedTexture = getEmbeddedTexture(scene, path);
    	if (embeddedTexture != nullptr) {
    		textureAssetInfo.embedded = embeddedTexture;
    	}
    	
        materialInfo.textures.push_back(textureAssetInfo);
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


std::shared_ptr<mesh_asset> getMesh(const aiMesh* mesh, const aiNode* node, const aiScene* scene, int meshIndex)
{
	if (!mesh->HasPositions())
	{
		throw std::runtime_error("mesh with name:" + std::string{mesh->mName.C_Str()} + " has no positions defined in it!");
	}

	auto transform =  node->mTransformation;
	auto vertexCount = mesh->mNumVertices;

	std::shared_ptr<std::vector<glm::vec3>> positions = getPositions(*mesh, transform, vertexCount);
	std::shared_ptr<std::vector<unsigned int>> indices = getIndices(*mesh);
	std::shared_ptr<std::vector<glm::vec3>> normals = getNormals(*mesh, transform, vertexCount);
	std::shared_ptr<std::vector<glm::vec2>> uv0 = getUV0(*mesh, transform, vertexCount);

	std::shared_ptr<std::vector<glm::vec3>> tangents =  getTangents(*mesh,transform,vertexCount);


	auto meshAsset = std::make_shared<mesh_asset>(meshIndex);
	meshAsset->positions = positions;
	meshAsset->normals = normals;
	meshAsset->tangents = tangents;
	meshAsset->indices = indices;
	meshAsset->UV0 = uv0;
	meshAsset->name = mesh->mName.C_Str();
	return meshAsset;
}

std::shared_ptr<mesh_asset> getMeshCached(int meshIndex, aiMesh* mesh, const aiNode* node, model_load_context& context)
{
	for(auto& cachedMesh : context.modelInfo->meshes)
	{
		if (cachedMesh->getSubMeshIndex() == meshIndex)
		{
			return cachedMesh;
		}
	}
	
	return getMesh(mesh, node, context.scene, meshIndex);
}

std::shared_ptr<material_asset> getMaterialCached(const aiMesh& mesh, model_load_context& context)
{
	if (mesh.mMaterialIndex >= 0)
	{
		//TODO can later use a map instead for O(1) time complexity 
		for (auto& mat : context.modelInfo->materials)
		{
			if(mat->index == mesh.mMaterialIndex)
			{
				return mat;
			}
		}

		auto materialInfo = std::make_shared<material_asset>();
		materialInfo->index = mesh.mMaterialIndex;
		auto& scene = *context.scene;
		auto& modelInfo = *context.modelInfo;
		auto const material = scene.mMaterials[mesh.mMaterialIndex];
		fetchTextures(*material, scene, *materialInfo, modelInfo, aiTextureType_DIFFUSE);
		fetchTextures(*material, scene, *materialInfo,modelInfo, aiTextureType_NORMALS);
		setShaders(*material, materialInfo->shaders);
		setMaterialProperties(*material, materialInfo->propertySet);
		return materialInfo;
	}
	else
	{
		return context.defaultMaterialAsset;
	}
}

void processNode(const aiNode* node, model_load_context& context)
{
	for (int i = 0; i < node->mNumMeshes; ++i) {
		int meshIndex = node->mMeshes[i];
		auto mesh = context.scene->mMeshes[meshIndex];
		context.modelInfo->meshes.push_back(getMeshCached(meshIndex, mesh, node, context));
		context.modelInfo->materials.push_back(getMaterialCached(*mesh, context));
	}

	for (int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], context);
	}
}


//TODO decouple input and output args 
void model_loader::load(std::string filePath, model_info& info)
{
    Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS);
	
	stopwatch stopwatch;
	stopwatch.start();
	
    const aiScene* scene = importer.ReadFile(filePath, processSteps);
    
    if (scene == nullptr) 
    {
        throw std::runtime_error(importer.GetErrorString() );
    }
	
	if (scene->mNumMeshes <= 0)
	{
		throw std::runtime_error("model in path:" + filePath + " has no mesh defined in it!");
	}

	stopwatch.stop();
	std::cout<< "loading model info with name: " << info.name << " took: " << stopwatch.getElapsedMiliSeconds() << " ms\n";
	stopwatch.start();
	
	model_load_context context;
	context.scene = scene;
	context.modelInfo = &info;
	processNode(scene->mRootNode, context);


	stopwatch.stop();
	std::cout<< "extracting model with name: " << info.name << " took: " << stopwatch.getElapsedMiliSeconds() << " ms\n";
}

std::shared_ptr<mesh_asset> model_loader::loadFirstMesh(std::string filePath) {
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, processSteps);

    if (scene == nullptr)
    {
        std::cerr << importer.GetErrorString() << std::endl;
        throw std::runtime_error("could not load the file");
    }
	
	auto firstMesh = scene->mMeshes[0];
    return getMesh(firstMesh, scene->mRootNode, scene, 0);
}

model_loader::model_loader()
{
	m_DefaultMaterialAsset = std::make_shared<material_asset>();
	m_DefaultMaterialAsset->shaders.emplace_back(m_PhongVertPath, shaderType::vertex);
	m_DefaultMaterialAsset->shaders.emplace_back(m_PhongFragPath, shaderType::fragment);

	Assimp::DefaultLogger::create("logger", Assimp::Logger::DEBUGGING);
	auto stream = Assimp::LogStream::createDefaultStream(aiDefaultLogStream_STDOUT);
	Assimp::DefaultLogger::get()->attachStream(stream);
}

model_loader::~model_loader()
{
	Assimp::DefaultLogger::kill();
}


