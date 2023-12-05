#include "light_directional.h"

void modelViewer::render::light_directional::setColor(glm::vec3 color) {
    m_Color = color;
}

glm::vec3 modelViewer::render::light_directional::getColor() const {
    return m_Color;
}

void modelViewer::render::light_directional::setPosition(glm::vec3 position) {
    m_Position = position;
}

glm::vec3 modelViewer::render::light_directional::getPosition() const {
    return m_Position;
}
