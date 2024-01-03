#include "object_factory.h"
#include "texture_cube.h"
#include "texture_2D.h"

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

std::vector<texture_binding> object_factory::getTextures(const model_info& info) {

	std::vector<texture_binding> textures;
	for (auto& textureInfo : info.material.textures ) {

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


std::shared_ptr<mesh> object_factory::getMesh(model_info &info) {

	if (info.mesh)
	{
		return std::make_shared<mesh>(info.mesh);
	}

	info.mesh = m_ModelLoader.loadMesh(info.path);
	auto meshPtr  = std::make_shared<mesh>(info.mesh);

	return meshPtr;
}


std::shared_ptr<shader_program> object_factory::getProgram(const model_info &info) {

	std::vector<shader> shaders;
	for (auto& shaderInfo : info.material.shaders) {

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

std::shared_ptr<render_object> object_factory::createObject(model_info& info)
{
	auto program = getProgram(info);
	auto mesh = getMesh(info);
	auto textures = getTextures(info);
	auto mat = std::make_shared<material>(info.material,textures, program, m_DefaultTextures);
	auto object = std::make_shared<render_object>(mat, mesh, info.name);

	object->setTransform(info.transform);
	
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
