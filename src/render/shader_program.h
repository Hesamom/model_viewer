#ifndef MODEL_VIEWER_SHADER_PROGRAM_H
#define MODEL_VIEWER_SHADER_PROGRAM_H

#include "../resource/shader_loader.h"
#include "shader.h"
#include "glm/glm.hpp"

namespace modelViewer::render{
    class shader_program {
        
private:
        unsigned int m_ProgramId = 0;
    public:
        shader_program(std::initializer_list<shader> shaders);
        shader_program(std::vector<shader>& shaders);
        ~shader_program();
		void validate();
        std::string getLinkLog();
        bool isLinked();
        void bind();
        int getAttributeLocation(const std::string& attributeName);
        int getUniformLocation(const std::string& uniformName);
        void setUniformVector3(int location, glm::vec3 vec3);
        void setUniformVector4(int location, glm::vec4 vec4);
        void setUniformFloat(int location, float value);
        void setUniformInt(int location, int value);
        void setUniformMatrix4(int index, glm::mat4& mat);
};
}


#endif
