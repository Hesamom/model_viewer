
#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "../resource/shader_loader.h"
#include "../resource/texture_loader.h"
#include "../resource/model_loader.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include "render_object.h"
#include "texture_setup.h"

namespace modelViewer::render
{

	class object_factory {

	private:
		res::model_loader m_ModelLoader;
		res::texture_loader m_TextureLoader;
		res::shader_loader m_ShaderLoader;

		std::shared_ptr<shader_program> getProgram(const res::model_info& info);
		std::shared_ptr<mesh> getMesh(res::model_info& info);

		std::shared_ptr<texture> createEmbeddedTexture(std::shared_ptr<res::texture_embedded> embedded, texture_setup setup);

		std::shared_ptr<texture> createTexture(const res::texture_asset_info& info);

		std::vector<texture_binding> getTextures(const res::model_info& info);
		
		std::unordered_map<shader_uniform_type, std::shared_ptr<texture>> m_DefaultTextures;
		
	public:
		object_factory();
		std::shared_ptr<render_object> createObject(res::model_info& info);
		res::shader_loader& getShaderLoader();
		res::model_loader& getModelLoader();
		res::texture_loader& getTextureLoader();
		std::unordered_map<shader_uniform_type, std::shared_ptr<texture>> getDefaultTextures();
	};


}

#endif //OBJECT_FACTORY_H
