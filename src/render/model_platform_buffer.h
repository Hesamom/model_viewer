
#ifndef MODEL_VIEWER_MODELPLATFORMBUFFER_H
#define MODEL_VIEWER_MODELPLATFORMBUFFER_H


#include "../resource/shader_loader.h"
#include "shader_program.h"
#include "render_object.h"

namespace modelViewer::render
{
    struct model_platform_info
    {
        float lineSpace = 0;
        int sizeX = 0;
        int sizeY = 0;
    };
    
    class model_platform_buffer {

    private:
        std::unique_ptr<modelViewer::render::render_object> m_Object;
        const std::filesystem::path m_FragShaderPath = "res/shaders/sample/lines_frag.glsl";
        const std::filesystem::path m_VertShaderPath = "res/shaders/sample/lines_vert.glsl";
        
    public:
       void init(modelViewer::res::shader_loader& shaderLoader, const model_platform_info& info);
       void draw(glm::mat4 viewProjection);
    };
}


#endif 
