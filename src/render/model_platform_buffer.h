
#ifndef MODEL_VIEWER_MODELPLATFORMBUFFER_H
#define MODEL_VIEWER_MODELPLATFORMBUFFER_H


#include "../resource/shader_loader.h"
#include "gl/shader_program.h"
#include "object_renderer.h"
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
		
		const std::string m_GridVertShaderPath = modelViewer::res::literals::shaders::line_vert;
        const std::string m_GridFragShaderPath = modelViewer::res::literals::shaders::line_frag;

		const std::string m_PlaneVertShaderPath = modelViewer::res::literals::shaders::lit_vert;
		const std::string m_PlaneFragShaderPath = modelViewer::res::literals::shaders::lit_frag;

		const std::string m_PlaneDiffuseTexture = modelViewer::res::literals::textures::default_white;
		
    public:
		std::shared_ptr<modelViewer::render::object_renderer> generateGrid(modelViewer::render::object_factory& objectFactory,
			const model_platform_info &info);
		std::shared_ptr<modelViewer::render::object_renderer> generatePlane(modelViewer::render::object_factory& objectFactory,
			const model_platform_info &info);
    };
}


#endif 
