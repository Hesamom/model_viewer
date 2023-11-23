
#ifndef MODEL_VIEWER_MESHASSET_H
#define MODEL_VIEWER_MESHASSET_H

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace modelViewer::res {
    
    
    class mesh_asset {
    public:
        std::vector<glm::vec3> m_Positions;
        std::vector<unsigned int> m_Indices;

        std::vector<glm::vec3> m_Normals;
        std::vector<glm::vec2> m_UV0;

        std::string m_Name;
        mesh_asset(std::vector<glm::vec3>& positions, std::vector<unsigned int>& indices, std::vector<glm::vec3>& 
                normals, std::vector<glm::vec2>& UV0) {
            m_Indices = indices;
            m_Positions = positions;
            m_Normals = normals;
            m_UV0 = UV0;
        }
    };
}

#endif //MODEL_VIEWER_MESHASSET_H
