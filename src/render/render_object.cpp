#include "render_object.h"

using namespace modelViewer::render;
using namespace modelViewer::common;

transform& render_object::getTransform() {
    return m_Transform;
}

void render_object::setTransform(transform &t) {
    m_Transform = t;
}


void render_object::render(glm::mat4 viewProjection) {
    
    m_ShaderProgram->bind();
    glm::mat4 Model = glm::mat4(1.0f);
    auto mvp = viewProjection * m_Transform.getMatrix();
    int mvpIndex = m_ShaderProgram->getUniformLocation("m_MVP");
    int modelIndex = m_ShaderProgram->getUniformLocation("m_Model");
    int textureIndex = m_ShaderProgram->getUniformLocation("m_TextureSampler");
    m_ShaderProgram->setUniformMatrix4(modelIndex, Model);
    m_ShaderProgram->setUniformMatrix4(mvpIndex, mvp);
    m_Texture->bind();
    m_ShaderProgram->setUniform1i(textureIndex,0);
    m_Mesh->bind();
    m_Mesh->draw();
}

render_object::render_object(std::shared_ptr<shader_program>& program, std::shared_ptr<mesh>& mesh,
                             std::shared_ptr<texture>& texture) {
    m_Mesh = mesh;
    m_ShaderProgram = program;
    m_Texture = texture;

    m_Mesh->bindAttributes(*m_ShaderProgram);
}

std::shared_ptr<shader_program>& render_object::getProgram() {
    return m_ShaderProgram;
}

