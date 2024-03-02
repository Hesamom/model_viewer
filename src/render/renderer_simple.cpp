
#include "renderer_simple.h"

modelViewer::render::renderer_simple::renderer_simple(std::shared_ptr<gfx_device>& device) {
    m_Device = device;
}

void modelViewer::render::renderer_simple::render(render_scene& scene, camera& camera, bool shadowEnabled,
                                                  bool reflectionEnabled) {

    m_Device->clearColorBuffer(m_Color);
    m_Device->clearDepthBuffer();
}

void modelViewer::render::renderer_simple::setClearFlag(glm::vec4 color) {

    m_Color = color;
    m_Device->setClearColor(m_Color);
}

void modelViewer::render::renderer_simple::setReflectionPosition(const glm::vec3& pos) {
    m_ReflectPos = pos;
}

glm::vec3& modelViewer::render::renderer_simple::getReflectionPosition() {
        return m_ReflectPos;
}

void modelViewer::render::renderer_simple::setClearMode(clear_mode mode) {
}

void modelViewer::render::renderer_simple::setReflectionClearMode(clear_mode mode) {
}

void modelViewer::render::renderer_simple::setReflectionClearFlag(const glm::vec4& color) {
}
