
#include "index_buffer.h"
#include "glew/include/GL/glew.h"

modelViewer::render::index_buffer::index_buffer(std::vector<unsigned int> &data) {
    
    glGenBuffers(1, &m_BufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  data.size() * sizeof (unsigned int), &data[0], GL_STATIC_DRAW);
    
    m_Count = data.size();
}

modelViewer::render::index_buffer::~index_buffer() {
    glDeleteBuffers(1, &m_BufferId);
}

void modelViewer::render::index_buffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
}



int modelViewer::render::index_buffer::getCount() const {
    return m_Count;
}

void modelViewer::render::index_buffer::drawShaded() {
    glDrawElements(GL_TRIANGLES, m_Count, GL_UNSIGNED_INT, 0);
}

void modelViewer::render::index_buffer::setName(const std::string &name) const {
    glObjectLabel(GL_BUFFER,m_BufferId,-1,name.data());
}

void modelViewer::render::index_buffer::drawWireframe() {
    glDrawElements(GL_LINE_STRIP, m_Count, GL_UNSIGNED_INT, 0);
}

