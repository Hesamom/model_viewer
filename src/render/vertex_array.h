
#ifndef MODEL_VIEWER_VERTEX_ARRAY_H
#define MODEL_VIEWER_VERTEX_ARRAY_H

#include "vertex_buffer.h"

namespace modelViewer::render {
    
    class vertex_array
    {
    private:
        unsigned int m_arrayId;
    public:
        vertex_array();
        void setBuffer(vertex_buffer& buffer, int layoutIndex);
        void bind();
    };
}

#endif
