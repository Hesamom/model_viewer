
#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "../resource/shader_loader.h"
#include "../resource/texture_loader.h"
#include "../resource/model_loader.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include "object_renderer.h"
#include "texture_setup.h"

namespace modelViewer::render
{

	class object_factory {

	private:
		res::model_loader m_ModelLoader;
		res::texture_loader m_TextureLoader;
		res::shader_loader m_ShaderLoader;
		std::unordered_map<std::shared_ptr<res::material_asset>, std::shared_ptr<material>> m_LoadedMaterials;

		std::shared_ptr<shader_program> getProgram(std::shared_ptr<res::material_asset> materialAsset);
		std::vector<std::shared_ptr<mesh>> getMeshes(res::model_info& info);

		std::shared_ptr<texture> createEmbeddedTexture(std::shared_ptr<res::texture_embedded> embedded, texture_setup setup);

		std::shared_ptr<texture> createTexture(const res::texture_asset_info& info);

		std::vector<texture_binding> getTextures(std::shared_ptr<res::material_asset> materialAsset);
		
		std::unordered_map<shader_uniform_type, std::shared_ptr<texture>> m_DefaultTextures;
		
	public:
		object_factory();
		std::shared_ptr<object_renderer> createObject(res::model_info& info);
		res::shader_loader& getShaderLoader();
		res::model_loader& getModelLoader();
		res::texture_loader& getTextureLoader();
		std::unordered_map<shader_uniform_type, std::shared_ptr<texture>> getDefaultTextures();

		std::vector<std::shared_ptr<material>> getMaterials(res::model_info& info);
		std::shared_ptr<material> getMaterial(const std::shared_ptr<res::material_asset>& asset);
	};


}

#endif //OBJECT_FACTORY_H
