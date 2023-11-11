
#ifndef MODEL_VIEWER_MESHASSET_H
#define MODEL_VIEWER_MESHASSET_H

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace modelViewer::res {
    
    
    class mesh_asset {
    public:
        glm::vec3 *m_Vertices = nullptr;
        glm::vec3 *m_Normals = nullptr;
        glm::vec2 *m_UV0 = nullptr;
        size_t m_verticesCount;

        mesh_asset(size_t verticesCount, glm::vec3 *vertices, glm::vec3 *normals, glm::vec2 *UV0) : 
        m_verticesCount{verticesCount} {
            m_Vertices = vertices;
            m_Normals = normals;
            m_UV0 = UV0;
        }
    };
}

#endif //MODEL_VIEWER_MESHASSET_H
