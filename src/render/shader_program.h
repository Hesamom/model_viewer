#ifndef MODEL_VIEWER_SHADER_PROGRAM_H
#define MODEL_VIEWER_SHADER_PROGRAM_H

#include "../resource/shader_loader.h"
#include "shader.h"
#include "glm/glm.hpp"

namespace modelViewer::render
{
    class shader_program
		{
private:
        unsigned int m_ProgramId = 0;
    public:
        shader_program(std::initializer_list<shader> shaders);
        shader_program(std::vector<shader>& shaders);
        ~shader_program();
		void validateLinking();
        std::string getLinkLog();
        bool isLinked();
        void bind();
        int getAttributeLocation(const std::string& attributeName);
        int getUniformLocation(const std::string& uniformName);
        void setUniform(int location, glm::vec3 vec3);
        void setUniform(int location, glm::vec4 vec4);
        void setUniform(int location, float value);
        void setUniform(int location, int value);
        void setUniform(int index, glm::mat4& mat);
};
}


#endif
