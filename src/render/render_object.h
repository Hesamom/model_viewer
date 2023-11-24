#ifndef MODEL_VIEWER_RENDER_OBJECT_H
#define MODEL_VIEWER_RENDER_OBJECT_H

#include "glm/glm.hpp"
#include "../common/transform.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"

namespace modelViewer::render
{
    class render_object {
    private:
        common::transform m_Transform{};
        
        std::shared_ptr<shader_program> m_ShaderProgram;
        std::shared_ptr<mesh> m_Mesh;
        std::shared_ptr<texture> m_Texture;
        
        std::string m_Name;
    public:
        render_object(std::shared_ptr<shader_program>& program , std::shared_ptr<mesh>& mesh,  
                      std::shared_ptr<texture>& 
                texture, const std::string& name);
        common::transform& getTransform();
        void setTransform(common::transform& t);
        std::shared_ptr<shader_program>& getProgram();
        
        void render(glm::mat4 viewProjection);
    };
}
#endif
