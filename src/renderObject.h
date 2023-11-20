#ifndef MODEL_VIEWER_RENDEROBJECT_H
#define MODEL_VIEWER_RENDEROBJECT_H


#include "glm/glm.hpp"
#include "shaderProgram.h"
#include "mesh.h"

namespace modelViewer::render{
    class renderObject {
    private:
        shader_program m_ShaderProgram;
        glm::mat4 m_Transform;
        mesh m_Mesh;
        texture texture;
        bool isDirty;

        void reCalculation();
    public:
        glm::mat3 getTransform();
        modelViewer::render::shaderProgram getShaderProgram();
        //TODO: need two external perameters like view and projection materix
        void render();
    };
}
#endif
