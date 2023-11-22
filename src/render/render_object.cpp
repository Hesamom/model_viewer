#include "render_object.h"
#include "gl/glew.h"

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
    int index = m_ShaderProgram->getUniformLocation("m_MVP");
    m_ShaderProgram->setUniformMatrix4(index,mvp);
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

