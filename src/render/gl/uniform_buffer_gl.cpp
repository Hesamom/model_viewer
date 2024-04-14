
#include "uniform_buffer_gl.h"
#include "glew/include/GL/glew.h"

modelViewer::render::uniform_buffer_gl::uniform_buffer_gl(int size, const std::string& name)
{
    //TODO check the min buffer size in docs
    if (size <= 0)
    {
        throw std::range_error("size out of range");
    }

    glGenBuffers(1, &m_BufferId);
    glBindBuffer(GL_UNIFORM_BUFFER, m_BufferId);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glObjectLabel(GL_TEXTURE, m_BufferId, -1, name.data());
    
    m_Size = size;
}

modelViewer::render::uniform_buffer_gl::~uniform_buffer_gl()
{
    glDeleteBuffers(1, &m_BufferId);
}

int modelViewer::render::uniform_buffer_gl::getSize()
{
    return m_Size;
}

void modelViewer::render::uniform_buffer_gl::setData(void* data, int offset, int size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_BufferId);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data); 
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void modelViewer::render::uniform_buffer_gl::bind(int slot)
{
    //TODO check the max slot using GL_MAX_UNIFORM_BUFFER_BINDINGS
    if (slot < 0 || slot >= 36)
    {
        throw std::range_error("slot must be more than 0");
    }
    //TODO later use a big buffer and bind using glBindBufferRange
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_BufferId); 
}
