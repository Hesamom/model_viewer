
#include "vertex_buffer.h"
#include "glew/include/GL/glew.h"

modelViewer::render::vertex_buffer::vertex_buffer(std::vector<glm::vec3>& data) {
    glGenBuffers(1, &m_BufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    m_Size = data.size();
    glBufferData(GL_ARRAY_BUFFER, m_Size * sizeof (glm::vec3), &data[0], GL_STATIC_DRAW);
}

modelViewer::render::vertex_buffer::~vertex_buffer() {
    glDeleteBuffers(1, &m_BufferId);
}

void modelViewer::render::vertex_buffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void modelViewer::render::vertex_buffer::draw() {
    glDrawArrays(GL_TRIANGLES, 0, m_Size);
}
