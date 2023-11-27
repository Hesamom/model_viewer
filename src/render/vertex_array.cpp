#include "vertex_array.h"
#include "gl/glew.h"

modelViewer::render::vertex_array::vertex_array() {
    glGenVertexArrays(1, &m_arrayId);
}


void modelViewer::render::vertex_array::bind() {
    glBindVertexArray(m_arrayId);
}

void modelViewer::render::vertex_array::unbind() {
    glBindVertexArray(0);
}
