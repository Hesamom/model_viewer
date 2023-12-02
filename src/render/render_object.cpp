#include "render_object.h"

using namespace modelViewer::render;
using namespace modelViewer::common;

transform& render_object::getTransform() {
    return m_Transform;
}

void render_object::setTransform(transform &t) {
    m_Transform = t;
}


void render_object::render(glm::mat4 view, glm::mat4 projection, render_mode mode) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, ("render: " + m_Name).c_str());
    
    m_ShaderProgram->bind();
    
    
    auto modelView = view * m_Transform.getMatrix();
    int modelViewIndex = m_ShaderProgram->getUniformLocation("m_MV");
    m_ShaderProgram->setUniformMatrix4(modelViewIndex, modelView);
    
    int projectionIndex = m_ShaderProgram->getUniformLocation("m_Projection");
    m_ShaderProgram->setUniformMatrix4(projectionIndex, projection);
    
    int index = 0;
    for(auto& t : m_Textures)
    {
        t->active(index);
        index++;
    }
    
    m_Mesh->bind();

    switch (mode)
    {
        case render_mode::triangles:
            m_Mesh->draw();
            break;
        case render_mode::lines:
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

void
render_object::addTexture(std::shared_ptr<texture> &texture, std::string samplerName) {
    
    m_Textures.push_back(texture);
    
    m_ShaderProgram->bind();
    auto loc = m_ShaderProgram->getUniformLocation(samplerName);
    m_ShaderProgram->setUniformInt(loc, m_BoundTextures);
    
    m_BoundTextures++;
}

void render_object::setMaterial(const modelViewer::res::material &material) {
}


