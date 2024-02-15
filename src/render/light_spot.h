
#ifndef LIGHT_SPOT_H
#define LIGHT_SPOT_H
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

namespace modelViewer::render {
    class object_renderer;

    class light_spot {
    public:
        glm::vec3 direction { 0};
        glm::vec3 position {0};
        glm::vec3 ambient{0};
        glm::vec3 diffuse{0};
        float innerCutoff = 0;
        float outerCutoff = 0;
        int id = 0;
        std::shared_ptr<object_renderer> object;
        glm::mat4x4 viewProjection;
    };
    
}

#endif //LIGHT_SPOT_H
