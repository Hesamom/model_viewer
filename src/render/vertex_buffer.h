
#ifndef MODEL_VIEWER_VERTEXBUFFER_H
#define MODEL_VIEWER_VERTEXBUFFER_H
#include "glm/glm.hpp"

namespace modelViewer::render {
    
    //TODO make it a template to support different data types
    class vertex_buffer {
    private:
        unsigned int m_BufferId;
        int m_Size;
    public:
        explicit vertex_buffer(std::vector <glm::vec3> &data);
        ~vertex_buffer();
        void bind();
        void draw();
    };
}

#endif
