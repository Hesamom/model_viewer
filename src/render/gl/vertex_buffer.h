
#ifndef MODEL_VIEWER_VERTEXBUFFER_H
#define MODEL_VIEWER_VERTEXBUFFER_H

#include "glew/include/GL/glew.h"


namespace modelViewer::render {
    
    template <typename T, int components>
    class vertex_buffer {
    private:
        static_assert(components > 0);
        unsigned int m_BufferId;
        int m_Size;
    public:

        typedef glm::vec<components, T, glm::defaultp>		valueType;
        
        explicit vertex_buffer(std::vector <glm::vec<components, T, glm::defaultp>> &data) : m_BufferId{0}, m_Size{0}
        {
            glGenBuffers(1, &m_BufferId);
            glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
            m_Size = static_cast<int>(data.size());
            glBufferData(GL_ARRAY_BUFFER, m_Size * sizeof (glm::vec<components, T, glm::defaultp>), &data[0], GL_STATIC_DRAW);
        }
        ~vertex_buffer()
        {
            glDeleteBuffers(1, &m_BufferId);
        }
        
        void setName(const std::string& name)
        {
            glObjectLabel(GL_BUFFER,m_BufferId,-1,name.data());    
        }
        
        constexpr int getGlType()
        {
            if constexpr (std::is_same<T, float>::value)
            {
                return GL_FLOAT;
            }
            if constexpr (std::is_same<T, int>::value)
            {
                return GL_INT;
            }
            if constexpr (std::is_same<T, double>::value)
            {
                return GL_DOUBLE;
            }
            if constexpr (std::is_same<T, short>::value)
            {
                return GL_SHORT;
            }
            if constexpr (std::is_same<T, unsigned char>::value)
            {
                return GL_BYTE;
            }
            else
            {
                static_assert("type not supported!");
            }
        }

        void bindBuffer(int layoutIndex)
        {
            bind();
            glVertexAttribPointer(layoutIndex,components,getGlType(),GL_FALSE,0, nullptr);
            glEnableVertexAttribArray(layoutIndex);
        }
        
        void bind()
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
        }
        void draw()
        {
            glDrawArrays(GL_TRIANGLES, 0, m_Size);
        }
    };
}

#endif
