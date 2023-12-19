
#ifndef MODEL_VIEWER_MODELPLATFORMBUFFER_H
#define MODEL_VIEWER_MODELPLATFORMBUFFER_H


#include "../resource/shader_loader.h"
#include "shader_program.h"
#include "render_object.h"
#include "renderable.h"
#include "object_factory.h"

namespace modelViewer::render
{
    struct model_platform_info
    {
        float lineSpace = 0;
        int sizeX = 0;
        int sizeZ = 0;
    };
    
    class model_platform_buffer {

    private:
		
        const std::string m_GridFragShaderPath = "res/shaders/sample/lines_frag.glsl";
        const std::string m_GridVertShaderPath = "res/shaders/sample/lines_vert.glsl";

		const std::string m_PlaneFragShaderPath = "res/shaders/sample/phong_phong_frag.glsl";
		const std::string m_PlaneVertShaderPath = "res/shaders/sample/phong_phong_vert.glsl";

		const std::string m_PlaneDiffuseTexture = "res/textures/default/white.png";
		
    public:
		std::shared_ptr<modelViewer::render::render_object> generateGrid(modelViewer::render::object_factory& objectFactory,
			const model_platform_info &info);
		std::shared_ptr<modelViewer::render::render_object> generatePlane(modelViewer::render::object_factory& objectFactory,
			const model_platform_info &info);
    };
}


#endif 
