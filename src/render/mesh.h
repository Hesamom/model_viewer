﻿
#ifndef MODEL_VIEWER_MESH_H
#define MODEL_VIEWER_MESH_H

#include "../resource/mesh_asset.h"
#include "shader_program.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_array.h"

namespace modelViewer::render {
    class mesh {
    private:
        vertex_buffer<float,3> m_PositionBuffer;
        vertex_buffer<float,3> m_NormalBuffer;
        vertex_buffer<float,2> m_UV0;
        index_buffer  m_IndexBuffer;
        vertex_array m_VertexArray;
        
        std::shared_ptr<modelViewer::res::mesh_asset> m_Asset;
    public:
        void bindAttributes(shader_program& program);
        void bind();
        void draw();
        explicit mesh(std::shared_ptr<modelViewer::res::mesh_asset>& asset);
        ~mesh();
    };
    
    
    
}


#endif //MODEL_VIEWER_MESH_H