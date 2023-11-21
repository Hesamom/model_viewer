
#ifndef MODEL_VIEWER_TRANSFORM_H
#define MODEL_VIEWER_TRANSFORM_H
#include "glm/glm.hpp"

namespace modelViewer::render {

    class transform {
    private:
        glm::vec3 m_Scale{1};
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;

    public:
        glm::mat4 getMatrix() const;
        glm::vec3 &getPosition();
        glm::vec3 &getRotation();
        glm::vec3 &getScale();
    };
}


#endif //MODEL_VIEWER_TRANSFORM_H
