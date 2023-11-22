
#include "transform.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace modelViewer::common;

glm::mat4 transform::getMatrix() const {
    
    auto identity = glm::mat4 (1);
    auto translation = glm::translate(identity, m_Position);
    float radian = glm::radians(1.0);
    auto rotation = m_Rotation.toMat4();
    auto scale = glm::scale(identity, m_Scale);
    auto transform = scale * translation * rotation;
    return transform;
}

glm::vec3 transform::getPosition() {
    return m_Position;
}

quaternion transform::getRotation() {
    return m_Rotation;
}

glm::vec3 transform::getScale() {
    return m_Scale;
}

void transform::setPosition(glm::vec3 position) {
    m_Position = position;
}

void transform::setRotation(const quaternion& rotation) {
    m_Rotation = rotation;
}

void transform::setScale(glm::vec3 scale) {
    m_Scale = scale;
}

glm::vec3 transform::getEularRotation() {
    return m_Rotation.toEuler();
}

void transform::setEularRotation(glm::vec3 eular) {
    m_Rotation = quaternion::fromEuler(eular);
}


