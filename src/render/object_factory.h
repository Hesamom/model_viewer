
#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#include "../resource/shader_loader.h"
#include "../resource/texture_loader.h"
#include "../resource/model_loader.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include "render_object.h"

namespace modelViewer::render
{

	class object_factory {

	private:
		modelViewer::res::model_loader m_ModelLoader;
		modelViewer::res::texture_loader m_TextureLoader;
		modelViewer::res::shader_loader m_ShaderLoader;

		std::shared_ptr<modelViewer::render::shader_program> getProgram(const modelViewer::res::model_info& info);
		std::shared_ptr<modelViewer::render::mesh> getMesh(modelViewer::res::model_info& info);
		std::vector<std::shared_ptr<modelViewer::render::texture>> getTextures(const modelViewer::res::model_info& info);
		
	public:
		std::shared_ptr<render_object> createObject(modelViewer::res::model_info& info);
		modelViewer::res::shader_loader& getShaderLoader();
		modelViewer::res::model_loader& getModelLoader();
	};


}

#endif //OBJECT_FACTORY_H
