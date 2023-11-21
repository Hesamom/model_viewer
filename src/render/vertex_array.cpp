
#include "vertex_array.h"
#include "glew/include/GL/glew.h"
#include "vertex_buffer.h"

modelViewer::render::vertex_array::vertex_array() {
    glGenVertexArrays(1, &m_arrayId);
}

void modelViewer::render::vertex_array::setBuffer(modelViewer::render::vertex_buffer &buffer, int layoutIndex) {
    bind();
    buffer.bind();
    glVertexAttribPointer(layoutIndex,3,GL_FLOAT,GL_FALSE,0, nullptr);
    glEnableVertexAttribArray(layoutIndex);
}

void modelViewer::render::vertex_array::bind() {
    glBindVertexArray(m_arrayId);
}
