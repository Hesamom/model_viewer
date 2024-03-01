#include "mesh_gl.h"
#include "../material.h"

void modelViewer::render::mesh_gl::bind() {
    m_VertexArray.bind();
}

modelViewer::render::mesh_gl::mesh_gl(std::shared_ptr<res::mesh_asset>& asset)
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

          if (m_Asset->tangents)
          {
              m_TangentsBuffer = std::make_unique<vertex_buffer<float,3>>(*m_Asset->tangents);
              m_TangentsBuffer->setName(m_Asset->name + "_tangents");
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

modelViewer::render::mesh_gl::~mesh_gl() {
    
}

void modelViewer::render::mesh_gl::bindAttributes(const modelViewer::render::material &material) {
    
    m_VertexArray.bind();
    
    int positionIndex = material.getAttributeLocation("v_position");
    if (positionIndex >= 0)
    {
        m_PositionBuffer->bindBuffer(positionIndex);
    }

    int uv0Index = material.getAttributeLocation("v_uv0");
    if (m_UV0 && uv0Index >= 0)
    {
        m_UV0->bindBuffer(uv0Index);
    }

    int colorIndex = material.getAttributeLocation("v_color");
    if (m_ColorBuffer && colorIndex >= 0)
    {
        m_ColorBuffer->bindBuffer(colorIndex);
    }
    
    int normalIndex = material.getAttributeLocation("v_normal");
    if(m_NormalBuffer && normalIndex >= 0)
    {
        m_NormalBuffer->bindBuffer(normalIndex);
    }

    int tangentIndex = material.getAttributeLocation("v_tangent");
    if(m_TangentsBuffer && tangentIndex >= 0)
    {
        m_TangentsBuffer->bindBuffer(tangentIndex);
    }
}

void modelViewer::render::mesh_gl::draw()
{
    m_IndexBuffer->drawShaded();
}

void modelViewer::render::mesh_gl::drawLines() {
    m_IndexBuffer->drawLines();
}

std::shared_ptr<modelViewer::res::mesh_asset> modelViewer::render::mesh_gl::getAsset() {
    return m_Asset;
}


