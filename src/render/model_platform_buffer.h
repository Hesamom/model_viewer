
#ifndef MODEL_VIEWER_MODELPLATFORMBUFFER_H
#define MODEL_VIEWER_MODELPLATFORMBUFFER_H

#include "object_renderer.h"
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
		
		const std::string m_GridVertShaderPath = res::literals::shaders::line_vert;
        const std::string m_GridFragShaderPath = res::literals::shaders::line_frag;

		const std::string m_PlaneVertShaderPath = res::literals::shaders::lit_vert;
		const std::string m_PlaneFragShaderPath = res::literals::shaders::lit_frag;

		const std::string m_PlaneDiffuseTexture = res::literals::textures::default_white;
		
    public:
		std::shared_ptr<object_renderer> generateGrid(object_factory& objectFactory,
		                                              const model_platform_info &info, std::shared_ptr<gfx_device>& device);
		std::shared_ptr<object_renderer> generatePlane(object_factory& objectFactory,
		                                                                    const model_platform_info &info, std::shared_ptr<gfx_device>& device);
    };
}


#endif 
