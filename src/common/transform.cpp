
#include "transform.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace modelViewer::render;

glm::mat4 transform::getMatrix() const {
    
    auto identity = glm::mat4 (1);
    auto translation = glm::translate(identity, m_Position);
    float radian = glm::radians(45.0);
    //TODO need to figure out how this works 
    //TODO use rotation field 
    auto rotation = glm::rotate(identity, radian, glm::vec3(1.0,1.0,1.0));
    auto scale = glm::scale(identity, m_Scale);
    auto transform = scale * translation * rotation;
    return transform;
}

glm::vec3 &transform::getPosition() {
    return m_Position;
}

glm::vec3 &transform::getRotation() {
    return m_Rotation;
}

glm::vec3 &transform::getScale() {
    return m_Scale;
}
