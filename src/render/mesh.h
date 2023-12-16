
#ifndef MODEL_VIEWER_MESH_H
#define MODEL_VIEWER_MESH_H

#include "../resource/mesh_asset.h"
#include "shader_program.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "material.h"

namespace modelViewer::render {
    class mesh {
    private:
        std::unique_ptr<vertex_buffer<float,3>> m_PositionBuffer;
        std::unique_ptr<vertex_buffer<float,4>> m_ColorBuffer;
        std::unique_ptr< vertex_buffer<float,3>> m_NormalBuffer;
        std::unique_ptr< vertex_buffer<float,3>> m_TangentsBuffer;
        std::unique_ptr<vertex_buffer<float,2>> m_UV0;
        std::unique_ptr<index_buffer>  m_IndexBuffer;
        vertex_array m_VertexArray;
        
        std::shared_ptr<modelViewer::res::mesh_asset> m_Asset;
    public:
        void bindAttributes(const modelViewer::render::material &material);
        void bind();
        void draw();
        void drawLines();
        std::shared_ptr<modelViewer::res::mesh_asset> getAsset();
        explicit mesh(std::shared_ptr<modelViewer::res::mesh_asset>& asset);
        ~mesh();
    };
    
    
    
}


#endif
