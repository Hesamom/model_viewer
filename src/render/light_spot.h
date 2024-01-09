
#ifndef LIGHT_SPOT_H
#define LIGHT_SPOT_H
#include <glm/vec3.hpp>

namespace modelViewer::render {
    class render_object;

    class light_spot {
    public:
        glm::vec3 direction { 0};
        glm::vec3 position {0};
        glm::vec3 ambient{0};
        glm::vec3 diffuse{0};
        float innerCutoff = 0;
        float outerCutoff = 0;
        int id = 0;
        std::shared_ptr<render_object> object;
    };
    
}

#endif //LIGHT_SPOT_H
