#ifndef MODEL_VIEWER_RENDER_OBJECT_H
#define MODEL_VIEWER_RENDER_OBJECT_H

#include "glm/glm.hpp"
#include "../common/transform.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include "../resource/material.h"

namespace modelViewer::render
{
    enum class render_mode
    {
        triangles,
        lines
    };
    
    class render_object {
        
    private:
        common::transform m_Transform{};
        
        std::shared_ptr<shader_program> m_ShaderProgram;
        std::shared_ptr<mesh> m_Mesh;
        std::shared_ptr<texture> m_Texture;
        
        std::string m_Name;
    public:
        render_object(std::shared_ptr<shader_program>& program , std::shared_ptr<mesh>& mesh, const std::string& name);
        common::transform& getTransform();
        void setTransform(common::transform& t);
        void setTexture(std::shared_ptr<texture>& texture);
        void setMaterial(const modelViewer::res::material& material);
        std::shared_ptr<shader_program>& getProgram();
        
        void render(glm::mat4 view, glm::mat4 projection, render_mode mode);
    };
}
#endif
