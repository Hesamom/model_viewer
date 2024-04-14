
#ifndef UNIFORM_BUFFER_GL_H
#define UNIFORM_BUFFER_GL_H
#include "../uniform_buffer.h"


namespace modelViewer::render
{
    class uniform_buffer_gl : public uniform_buffer
    {
    public:
        uniform_buffer_gl(int size, const std::string& name);
        ~uniform_buffer_gl();
        int getSize() override;
        void setData(void* data, int offset, int size) override;

        void bind(int slot);
    private:
        int m_Size = 0;
        unsigned int m_BufferId = -1;
    };
}



#endif //UNIFORM_BUFFER_GL_H
