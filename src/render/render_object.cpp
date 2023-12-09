﻿#include "render_object.h"

using namespace modelViewer::render;
using namespace modelViewer::common;

transform& render_object::getTransform() {
    return m_Transform;
}

void render_object::setTransform(const transform &t) {
    m_Transform = t;
}


void render_object::render(glm::mat4 view, glm::mat4 projection, render_mode mode) {

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, ("render: " + m_Name).c_str());
    
    
    m_Material->bind();
    auto model =  m_Transform.getMatrix();
    auto modelView = view * model;
    m_Material->setModel(model);
    m_Material->setModelView(modelView);
    m_Material->setProjection(projection);

	//TODO set material to use shadow map
	
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

std::shared_ptr<material> &render_object::getMaterial() {
    return m_Material;
}

render_object::render_object(std::shared_ptr<material>& material, std::shared_ptr<mesh>& mesh, const std::string& name) {
    m_Mesh = mesh;
    m_Material = material;
    m_Name = name;
    m_Mesh->bindAttributes(*m_Material);
}

void render_object::setLight(const light_directional &light) {
    m_Material->setLight(light);
}

void render_object::renderShadow()
{
	m_Mesh->bind();
	m_Mesh->draw();
}



