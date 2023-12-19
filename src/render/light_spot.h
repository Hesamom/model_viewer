
#ifndef LIGHT_SPOT_H
#define LIGHT_SPOT_H
#include <glm/vec3.hpp>

namespace modelViewer::render {
    class light_spot {
    public:
        glm::vec3 direction { 0};
        glm::vec3 position {0};
        glm::vec3 ambient{0};
        glm::vec3 diffuse{0};
        float innerCutoff = 0;
        float outerCutoff = 0;
    };
    
}

#endif //LIGHT_SPOT_H
