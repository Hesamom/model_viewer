
#ifndef MESH_GL_H
#define MESH_GL_H

#include "../../resource/mesh_asset.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"
#include "../mesh.h"
#include "shader_program_gl.h"

namespace modelViewer::render {
    class mesh_gl : public mesh {

    public:
        void bindLayout(const std::shared_ptr<shader_program> &program) override;
        void bind() override;
        void drawTriangles() override;
        void drawLines() override;
        std::shared_ptr<res::mesh_asset> getAsset() override;
        explicit mesh_gl(std::shared_ptr<res::mesh_asset>& asset);
        ~mesh_gl() override;

    private:
        std::unique_ptr<vertex_buffer<float,3>> m_PositionBuffer; //required
        std::unique_ptr<vertex_buffer<float,4>> m_ColorBuffer; //opt
        std::unique_ptr< vertex_buffer<float,3>> m_NormalBuffer; //opt
        std::unique_ptr< vertex_buffer<float,3>> m_TangentsBuffer; //opt
        std::unique_ptr<vertex_buffer<float,2>> m_UV0; //opt
		
        std::unique_ptr<index_buffer>  m_IndexBuffer;
        vertex_array m_VertexArray;
        
        std::shared_ptr<res::mesh_asset> m_Asset;
    };
    
    
    
}


#endif
