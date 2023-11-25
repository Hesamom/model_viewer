﻿
#ifndef MODEL_VIEWER_TRANSFORM_H
#define MODEL_VIEWER_TRANSFORM_H
#include "glm/glm.hpp"
#include "quaternion.h"

namespace modelViewer::common {

    class transform {
    private:
        glm::vec3 m_Scale{1};
        glm::vec3 m_Position{0};
        quaternion m_Rotation;

    public:
        glm::mat4 getMatrix() const;
        glm::vec3 getPosition();
        quaternion getRotation();
        glm::vec3 getEularRotation();
        glm::vec3 getScale();

        void setPosition(glm::vec3 position);
        void setEularRotation(glm::vec3 eular);
        void setRotation(const quaternion& rotation);
        void setScale(glm::vec3 scale);
    };
}


#endif //MODEL_VIEWER_TRANSFORM_H