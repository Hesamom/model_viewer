
#include "object_factory.h"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;


std::vector<std::shared_ptr<modelViewer::render::texture>> object_factory::getTextures(const model_info &info) {

	std::vector<std::shared_ptr<modelViewer::render::texture>> textures;
	for (auto& textureInfo : info.material.textures ) {
		if (textureInfo.path.empty())
		{
			continue;
		}

		auto textureAsset = m_TextureLoader.load(textureInfo.path, 4);

		texture_setup setup;
		setup.asset = textureAsset;
		setup.isMipMapActive = true;
		setup.mipMapMaxLevel = 1000;
		setup.mipMapMinLevel = 0;
		setup.type = textureInfo.type;
		//TODO set wrap mode
		auto texturePtr = std::make_shared<texture>(setup);
		textures.push_back(texturePtr);
	}

	return textures;
}


std::shared_ptr<modelViewer::render::mesh> object_factory::getMesh(model_info &info) {

	if (info.mesh)
	{
		return std::make_shared<mesh>(info.mesh);
	}

	info.mesh = m_ModelLoader.loadMesh(info.path);
	auto meshPtr  = std::make_shared<mesh>(info.mesh);

	return meshPtr;
}


std::shared_ptr<modelViewer::render::shader_program> object_factory::getProgram(const model_info &info) {

	std::vector<shader> shaders;
	for (auto& shaderInfo : info.material.shaders) {

		auto shaderAsset = m_ShaderLoader.load(shaderInfo.path, shaderInfo.type);
		shader shader(shaderAsset);
		shader.compile();
		shader.verify();

		shaders.push_back(shader);
	}

	auto program = std::make_shared<shader_program>(shaders);
	program->validate();
	return program;
}

std::shared_ptr<render_object> object_factory::createObject(model_info& info)
{
	auto program = getProgram(info);
	auto mesh = getMesh(info);
	auto textures = getTextures(info);
	auto mat = std::make_shared<material>(info.material,textures, program);
	auto object = std::make_shared<render_object>(mat, mesh, info.name);

	object->setTransform(info.transform);
	
	return object;
}

modelViewer::res::shader_loader& object_factory::getShaderLoader()
{
	return m_ShaderLoader;
}

modelViewer::res::model_loader& object_factory::getModelLoader()
{
	return m_ModelLoader;
}
