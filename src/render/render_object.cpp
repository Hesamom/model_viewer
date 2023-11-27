#include "render_object.h"

using namespace modelViewer::render;
using namespace modelViewer::common;

transform& render_object::getTransform() {
    return m_Transform;
}

void render_object::setTransform(transform &t) {
    m_Transform = t;
}


void render_object::render(glm::mat4 viewProjection, render_mode mode) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, ("render: " + m_Name).c_str());
    
    m_ShaderProgram->bind();
    auto mvp = viewProjection * m_Transform.getMatrix();
    int mvpIndex = m_ShaderProgram->getUniformLocation("m_MVP");
    m_ShaderProgram->setUniformMatrix4(mvpIndex, mvp);
    
    if(m_Texture)
    {
        //TODO need to move this to texture class
        int textureIndex = m_ShaderProgram->getUniformLocation("m_TextureSampler");
        m_Texture->bind();
        m_ShaderProgram->setUniformInt(textureIndex, 0);
    }
    
    m_Mesh->bind();

    switch (mode)
    {
        case render_mode::triangles:
            m_Mesh->draw();
            break;
        case render_mode::lines:
            glLineWidth(2);
            m_Mesh->drawLines();
            break;
    }
 

    glPopDebugGroup();
}

render_object::render_object(std::shared_ptr<shader_program>& program, std::shared_ptr<mesh>& mesh, const std::string& name) {
    m_Mesh = mesh;
    m_ShaderProgram = program;
    m_Name = name;
    m_Mesh->bindAttributes(*m_ShaderProgram);
}

std::shared_ptr<shader_program>& render_object::getProgram() {
    return m_ShaderProgram;
}

void render_object::setTexture(std::shared_ptr<texture> &texture) {
    m_Texture = texture;
}


