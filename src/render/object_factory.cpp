#include "object_factory.h"
#include "../common/stopwatch.h"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;


std::shared_ptr<texture_asset> object_factory::createEmbeddedTexture(std::shared_ptr<texture_embedded> embedded) {

	
	if (embedded == nullptr) {
		return nullptr;
	}
	
	if (embedded->isCompressed) {
		auto textureAsset = m_TextureLoader.loadFromMemmory(embedded->data, embedded->dataSize, embedded->channelsCount, true);
		return textureAsset;
	}
		
	textureInfo info;
	info.desiredChannels = embedded->channelsCount;
	info.width = embedded->width;
	info.height = embedded->height;
	info.forceFlip = true;
	auto textureAsset = std::make_shared<texture_asset>(embedded->data, info, "");
	return textureAsset;
		
}

std::shared_ptr<texture> object_factory::createTexture(const texture_asset_info& info) {
	
	texture_setup setup;
	//TODO set wrap mode
	setup.isMipMapActive = false;
	setup.isHeightMap = info.isHightMap;
	setup.mipMapMaxLevel = 1000;
	setup.mipMapMinLevel = 0;
	setup.type = info.type;

	auto embeddedTexture = createEmbeddedTexture(info.embedded);
	if (embeddedTexture != nullptr) {
		setup.assets.push_back(embeddedTexture);
		return m_Device->createTexture2D(setup);
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
	if (info.type == texture_asset_type::texture2D)
	{
		texturePtr = m_Device->createTexture2D(setup);
	}
	else
	{
		texturePtr = m_Device->createTextureCube(setup);
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
	auto mat = std::make_shared<material>(m_Device, *asset, textures, program, m_DefaultTextures);
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
			
			auto m = m_Device->createMesh(meshAsset);
			meshes.push_back(m);
		}
		return meshes;
	}

	auto meshAsset = m_ModelLoader.loadFirstMesh(info.path);
	info.meshes.push_back(meshAsset);
	auto meshPtr  = m_Device->createMesh(meshAsset);
	meshes.push_back(meshPtr);
	
	return meshes;
}


std::shared_ptr<shader_program> object_factory::getProgram(std::shared_ptr<material_asset> materialAsset) {

	std::vector<std::shared_ptr<shader_asset>> assets;
	for (auto& shaderInfo : materialAsset->shaders) {

		auto shaderAsset = m_ShaderLoader.load(shaderInfo.path, shaderInfo.type);
		assets.push_back(shaderAsset);
	}

	auto program = m_Device->createProgram(assets);
	return program;
}

object_factory::object_factory(std::shared_ptr<gfx_device>& device) {

	m_Device = device;
	createDefaultTextures();
}

void object_factory::createDefaultTextures()
{
	std::vector<std::pair<shader_uniform_texture_pair, std::string>> assets;
	shader_uniform_texture_pair diffuseSampler { shader_texture_type::texture2D, shader_texture_usage::diffuse};
	assets.emplace_back(diffuseSampler, literals::textures::default_white);

	shader_uniform_texture_pair defaultSampler { shader_texture_type::texture2D, shader_texture_usage::none};
	assets.emplace_back(defaultSampler, literals::textures::default_white);

	shader_uniform_texture_pair specularSampler { shader_texture_type::texture2D, shader_texture_usage::specular};
	assets.emplace_back(specularSampler, literals::textures::default_white);

	shader_uniform_texture_pair normalSampler { shader_texture_type::texture2D, shader_texture_usage::normal};
	assets.emplace_back(normalSampler, literals::textures::default_normal);

	shader_uniform_texture_pair cubeSampler { shader_texture_type::textureCube, shader_texture_usage::diffuse};
	assets.emplace_back(cubeSampler, literals::textures::default_white);

	for (const auto & asset : assets) {
		texture_asset_info info;
		
		switch (asset.first.type) {
			
			case shader_texture_type::texture2D:
				info.paths.push_back(asset.second);
				info.type = texture_asset_type::texture2D;
				break;
			case shader_texture_type::textureCube:
				for (int i = 0; i < 6; ++i)
				{
					info.paths.push_back(asset.second);
				}
				info.type = texture_asset_type::textureCube;
			break;
			default:
				throw std::runtime_error("not implemented yet!");
		}
		
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

std::map<shader_uniform_texture_pair, std::shared_ptr<texture>> object_factory::getDefaultTextures() {
	return m_DefaultTextures;
}


