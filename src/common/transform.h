
#ifndef MODEL_VIEWER_TRANSFORM_H
#define MODEL_VIEWER_TRANSFORM_H
#include "glm/glm.hpp"

namespace modelViewer::common {

    class transform {
    private:
        glm::vec3 m_Scale{1};
        glm::vec3 m_Position{0};
        glm::vec3 m_Rotation{0};

    public:
        glm::mat4 getMatrix() const;
        glm::vec3 getPosition();
        glm::vec3 getRotation();
        glm::vec3 getScale();

        void setPosition(glm::vec3 position);
        void setRotation(glm::vec3 eular);
        void setScale(glm::vec3 scale);
    };
}


#endif //MODEL_VIEWER_TRANSFORM_H
