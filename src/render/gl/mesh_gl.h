
#ifndef MODEL_VIEWER_MESH_H
#define MODEL_VIEWER_MESH_H

#include "../../resource/mesh_asset.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "../material.h"
#include "../mesh.h"

namespace modelViewer::render {
    class mesh_gl : public mesh {

    public:
        void bindAttributes(const material &material) override;
        void bind() override;
        void draw() override;
        void drawLines() override;
        std::shared_ptr<res::mesh_asset> getAsset() override;
        explicit mesh_gl(std::shared_ptr<res::mesh_asset>& asset);
        ~mesh_gl() override;

    private:
        std::unique_ptr<vertex_buffer<float,3>> m_PositionBuffer;
        std::unique_ptr<vertex_buffer<float,4>> m_ColorBuffer;
        std::unique_ptr< vertex_buffer<float,3>> m_NormalBuffer;
        std::unique_ptr< vertex_buffer<float,3>> m_TangentsBuffer;
        std::unique_ptr<vertex_buffer<float,2>> m_UV0;
        std::unique_ptr<index_buffer>  m_IndexBuffer;
        vertex_array m_VertexArray;
        
        std::shared_ptr<res::mesh_asset> m_Asset;
    };
    
    
    
}


#endif
