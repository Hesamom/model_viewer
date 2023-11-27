#include "mesh.h"

void modelViewer::render::mesh::bind() {
    m_VertexArray.bind();
}

modelViewer::render::mesh::mesh(std::shared_ptr<modelViewer::res::mesh_asset>& asset)
    : m_Asset{asset}
      {
          m_VertexArray.bind();
          m_PositionBuffer = std::make_unique<vertex_buffer<float,3>>(*m_Asset->positions);
          m_PositionBuffer->setName(m_Asset->name + "_pos");

          m_IndexBuffer = std::make_unique<index_buffer>(*m_Asset->indices);
          m_IndexBuffer->setName(m_Asset->name + "_index");
          
          if (m_Asset->normals)
          {
              m_NormalBuffer = std::make_unique<vertex_buffer<float,3>>(*m_Asset->normals);
              m_NormalBuffer->setName(m_Asset->name + "_normal");
          }

          if (m_Asset->colors)
          {
              m_ColorBuffer = std::make_unique<vertex_buffer<float,4>>(*m_Asset->colors);
              m_ColorBuffer->setName(m_Asset->name + "_color");
          }

          if (m_Asset->UV0)
          {
              m_UV0 = std::make_unique<vertex_buffer<float,2>>(*m_Asset->UV0);
              m_UV0->setName(m_Asset->name + "_uv0");
          }
          m_VertexArray.unbind();
   
}

modelViewer::render::mesh::~mesh() = default;

void modelViewer::render::mesh::bindAttributes(modelViewer::render::shader_program &program) {
    
    m_VertexArray.bind();
    
    int positionIndex = program.getAttributeLocation("v_position");
    if (positionIndex >= 0)
    {
        m_PositionBuffer->bindBuffer(positionIndex);
    }

    int uv0Index = program.getAttributeLocation("v_uv0");
    if (m_UV0 && uv0Index >= 0)
    {
        m_UV0->bindBuffer(uv0Index);
    }

    int colorIndex = program.getAttributeLocation("v_color");
    if (m_ColorBuffer && colorIndex >= 0)
    {
        m_ColorBuffer->bindBuffer(colorIndex);
    }
    
    int normalIndex = program.getAttributeLocation("v_normal");
    if(m_NormalBuffer && normalIndex >= 0)
    {
        m_NormalBuffer->bindBuffer(normalIndex);
    }

}

void modelViewer::render::mesh::draw()
{
    m_IndexBuffer->drawShaded();
}

void modelViewer::render::mesh::drawLines() {
    m_IndexBuffer->drawLines();
}


