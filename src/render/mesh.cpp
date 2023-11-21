#include <GL/glew.h>
#include "mesh.h"

void modelViewer::render::mesh::bind() {
    m_VertexArray.bind();
}

modelViewer::render::mesh::mesh(std::shared_ptr<modelViewer::res::mesh_asset>& asset)
    : m_Asset{asset},
      m_PositionBuffer{asset->m_Positions} ,
      m_NormalBuffer{asset->m_Normals},
      m_IndexBuffer{asset->m_Indices} 
      {
}

modelViewer::render::mesh::~mesh() = default;

void modelViewer::render::mesh::bindAttributes(modelViewer::render::shader_program &program) {
    
    int positionIndex = program.getAttributeLocation("v_position");
    if (positionIndex >= 0)
    {
        m_VertexArray.setBuffer(m_PositionBuffer, positionIndex);
    }
    
    int normalIndex = program.getAttributeLocation("v_normal");
    if(normalIndex >= 0)
    {
        m_VertexArray.setBuffer(m_NormalBuffer, normalIndex);
    }

}

void modelViewer::render::mesh::drawIndexed() {
    
    m_IndexBuffer.bind();
    m_IndexBuffer.draw();
}

void modelViewer::render::mesh::draw() {
    
    m_PositionBuffer.bind();
    m_PositionBuffer.draw();
}


