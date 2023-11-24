#include "mesh.h"

void modelViewer::render::mesh::bind() {
    m_VertexArray.bind();
}

modelViewer::render::mesh::mesh(std::shared_ptr<modelViewer::res::mesh_asset>& asset)
    : m_Asset{asset},
      m_PositionBuffer{asset->m_Positions} ,
      m_NormalBuffer{asset->m_Normals},
      m_UV0{asset->m_UV0},
      m_IndexBuffer{asset->m_Indices}
      {

          m_PositionBuffer.setName(m_Asset->m_Name + "_pos");
          m_NormalBuffer.setName(m_Asset->m_Name + "_normal");
          m_UV0.setName(m_Asset->m_Name + "_uv0");
          m_IndexBuffer.setName(m_Asset->m_Name + "_index");
}

modelViewer::render::mesh::~mesh() = default;

void modelViewer::render::mesh::bindAttributes(modelViewer::render::shader_program &program) {
    
    m_VertexArray.bind();
    
    int positionIndex = program.getAttributeLocation("v_position");
    if (positionIndex >= 0)
    {
        m_PositionBuffer.bindBuffer(positionIndex);
    }

    int uv0Index = program.getAttributeLocation("v_uv0");
    if (uv0Index >= 0)
    {
        m_UV0.bindBuffer(uv0Index);
    }
    
    int normalIndex = program.getAttributeLocation("v_normal");
    if(normalIndex >= 0)
    {
        m_NormalBuffer.bindBuffer(normalIndex);
    }

}

void modelViewer::render::mesh::draw() {

    if(m_IndexBuffer.getCount() != 0){
        m_IndexBuffer.bind();
        m_UV0.bind();
        m_NormalBuffer.bind();
        m_IndexBuffer.draw();
    } else{
        m_PositionBuffer.bind();
        m_UV0.bind();
        m_NormalBuffer.bind();
        m_PositionBuffer.draw();
    }
}


