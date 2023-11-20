#include "renderObject.h"

using namespace modelViewer::render;

void renderObject::reCalculation() {

}

glm::mat3 renderObject::getTransform() {
    if (isDirty)
    {
        reCalculation();
        isDirty = false;
    }

    return m_Transform;
}

modelViewer::render::shaderProgram renderObject::getShaderProgram() {
    return m_ShaderProgram;
}

void renderObject::render() {

}
