#include "object_factory.h"
#include "texture_cube.h"
#include "texture_2D.h"
#include "../common/stopwatch.h"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;


std::shared_ptr<texture> object_factory::createEmbeddedTexture(std::shared_ptr<texture_embedded> embedded, texture_setup setup) {

	
	if (embedded == nullptr) {
		return nullptr;
	}
	
	if (embedded->isCompressed) {
		auto textureAsset = m_TextureLoader.loadFromMemmory(embedded->data, embedded->dataSize, embedded->channelsCount, true);
		setup.assets.emplace_back(textureAsset);
		auto texturePtr = std::make_shared<texture_2D>(setup);
		return texturePtr;
	}
		
	textureInfo info;
	info.channels = embedded->channelsCount;
	info.width = embedded->width;
	info.height = embedded->height;
	info.forceFlip = true;
	auto textureAsset = std::make_shared<texture_asset>(embedded->data, info, "");
	setup.assets.emplace_back(textureAsset);
	auto texturePtr = std::make_shared<texture_2D>(setup);
	return texturePtr;
		
}

std::shared_ptr<texture> object_factory::createTexture(const texture_asset_info& info) {
	
	texture_setup setup;
	//TODO set wrap mode
	setup.isMipMapActive = true;
	setup.isHightMap = info.isHightMap;
	setup.mipMapMaxLevel = 1000;
	setup.mipMapMinLevel = 0;
	setup.type = info.type;

	auto embededTexture = createEmbeddedTexture(info.embedded, setup);
	if (embededTexture != nullptr) {
		return embededTexture;
	}

	if (info.paths.empty())
	{
		return nullptr;
	}
	
	for (auto &path: info.paths)
	{
		try {
			auto textureAsset = m_TextureLoader.load(path, 4, info.forceFlip);
			setup.assets.emplace_back(textureAsset);
		}
		catch (std::exception& exception) {
			std::cerr << exception.what() << std::endl;
		}
	}
	
	if (setup.assets.empty()) {
		return nullptr;
	}

	std::shared_ptr<texture> texturePtr = nullptr;
	if (info.type == texture_asset_type::textureCube)
	{
		texturePtr = std::make_shared<texture_cube>(setup);
	}
	else
	{
		texturePtr = std::make_shared<texture_2D>(setup);
	}
	
	return texturePtr;
}

std::vector<texture_binding> object_factory::getTextures(std::shared_ptr<material_asset> materialAsset) {

	std::vector<texture_binding> textures;
	for (auto& textureInfo : materialAsset->textures ) {

		auto texture = createTexture(textureInfo);
		if (texture == nullptr) {
			continue;
		}
		
		texture_binding binding;
		binding.texture = texture;
		binding.samplerName = textureInfo.samplerName;
		textures.push_back(binding);
	}

	return textures;
}

std::shared_ptr<material> object_factory::getMaterial(const std::shared_ptr<material_asset>& asset)
{
	if (m_LoadedMaterials.contains(asset))
	{
		return m_LoadedMaterials[asset];
	}

	auto program = getProgram(asset);
	auto textures = getTextures(asset);
	auto mat = std::make_shared<material>(*asset, textures, program, m_DefaultTextures);
	
	m_LoadedMaterials[asset] = mat;
	return mat;
}

std::vector<std::shared_ptr<material>> object_factory::getMaterials(model_info& info)
{
	std::vector<std::shared_ptr<material>> materials;

	if (info.materials.empty())
	{
		throw std::runtime_error("could not find any material in model with name: " + info.name);
	}

	for (auto& materialAsset : info.materials) {
		materials.push_back(getMaterial(materialAsset));
	}
	
	return materials;
}


std::vector<std::shared_ptr<mesh>> object_factory::getMeshes(model_info & info) {

	std::vector<std::shared_ptr<mesh>> meshes;
	
	if (!info.meshes.empty())
	{
		//TODO can cache already loaded meshes 
		for (auto& meshAsset : info.meshes) {
			
			auto m = std::make_shared<mesh>(meshAsset);
			meshes.push_back(m);
		}
		return meshes;
	}

	auto meshAsset = m_ModelLoader.loadFirstMesh(info.path);
	info.meshes.push_back(meshAsset);
	auto meshPtr  = std::make_shared<mesh>(meshAsset);
	meshes.push_back(meshPtr);
	
	return meshes;
}


std::shared_ptr<shader_program> object_factory::getProgram(std::shared_ptr<material_asset> materialAsset) {

	std::vector<shader> shaders;
	for (auto& shaderInfo : materialAsset->shaders) {

		auto shaderAsset = m_ShaderLoader.load(shaderInfo.path, shaderInfo.type);
		shader shader(shaderAsset);
		shader.compile();
		shader.verify();

		shaders.push_back(shader);
	}

	auto program = std::make_shared<shader_program>(shaders);
    program->validateLinking();
	return program;
}

object_factory::object_factory() {
	
	std::unordered_map<shader_uniform_type, std::string> assets;
	assets[shader_uniform_type::sampler2D] = "res/textures/default/white.png";

	for (const auto & asset : assets) {
		texture_asset_info info;
		info.paths.push_back(asset.second);
		//TODO set asset type to support cube and texture3D
		info.type = texture_asset_type::texture2D;

		const auto texture = createTexture(info);
		m_DefaultTextures[asset.first] = texture;
	}
}

std::shared_ptr<object_renderer> object_factory::createObject(model_info& info)
{
	stopwatch stopwatch;
	stopwatch.start();
	auto meshes = getMeshes(info);
	auto materials = getMaterials(info);
	auto object = std::make_shared<object_renderer>(materials, meshes, info.name);

	object->setTransform(info.transform);
	
	stopwatch.stop();
	std::cout<< "created object with name: " << info.name << " took " << stopwatch.getElapsedMiliSeconds() << " ms \n";
	return object;
}

shader_loader& object_factory::getShaderLoader()
{
	return m_ShaderLoader;
}

model_loader& object_factory::getModelLoader()
{
	return m_ModelLoader;
}

texture_loader& object_factory::getTextureLoader()
{
	return m_TextureLoader;
}

std::unordered_map<shader_uniform_type, std::shared_ptr<texture>> object_factory::getDefaultTextures() {
	return m_DefaultTextures;
}


