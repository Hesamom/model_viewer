
#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "../resource/shader_loader.h"
#include "../resource/texture_loader.h"
#include "../resource/model_loader.h"
#include "object_renderer.h"
#include "gfx_device.h"

namespace modelViewer::render
{

	class object_factory {

	private:
		res::model_loader m_ModelLoader;
		res::texture_loader m_TextureLoader;
		res::shader_loader m_ShaderLoader;
		std::shared_ptr<gfx_device> m_Device;
		std::unordered_map<std::shared_ptr<res::material_asset>, std::shared_ptr<material>> m_LoadedMaterials;

		std::shared_ptr<shader_program> getProgram(std::shared_ptr<res::material_asset> materialAsset);
		std::vector<std::shared_ptr<mesh>> getMeshes(res::model_info& info);

		std::shared_ptr<res::texture_asset> createEmbeddedTexture(std::shared_ptr<res::texture_embedded> embedded);

		std::shared_ptr<texture> createTexture(const res::texture_asset_info& info);

		std::vector<texture_binding> getTextures(std::shared_ptr<res::material_asset> materialAsset);
		
		std::map<shader_uniform_texture_pair, std::shared_ptr<texture>> m_DefaultTextures;
		
	public:
		explicit object_factory(std::shared_ptr<gfx_device>& device);
		std::shared_ptr<object_renderer> createObject(res::model_info& info);
		res::shader_loader& getShaderLoader();
		res::model_loader& getModelLoader();
		res::texture_loader& getTextureLoader();
		std::map<shader_uniform_texture_pair, std::shared_ptr<texture>> getDefaultTextures();

		std::vector<std::shared_ptr<material>> getMaterials(res::model_info& info);
		std::shared_ptr<material> getMaterial(const std::shared_ptr<res::material_asset>& asset);

		void createDefaultTextures();
	};


}

#endif //OBJECT_FACTORY_H
