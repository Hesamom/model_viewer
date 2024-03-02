#ifndef SHADER_PROGRAM_GL_H
#define SHADER_PROGRAM_GL_H

#include "../../resource/shader_loader.h"
#include "shader_gl.h"
#include "../shader_program.h"
#include "glm/glm.hpp"

namespace modelViewer::render
{
    class shader_program_gl : public shader_program
		{

    public:
        shader_program_gl(std::initializer_list<shader_gl> shaders);
        explicit shader_program_gl(std::vector<shader_gl>& shaders);
        ~shader_program_gl() override;
		void validateLinking() override;
        std::string getLinkLog() override;
        bool isLinked() override;
        void bind() override;
        int getAttributeLocation(const std::string& attributeName) override;
        int getUniformLocation(const std::string& uniformName) override;
    	
        void setUniform(int location, glm::vec3 vec3) override;
        void setUniform(int location, glm::vec4 vec4) override;
        void setUniform(int location, float value) override;
        void setUniform(int location, int value) override;
        void setUniform(int index, glm::mat4& mat) override;
        std::vector<shader_uniform_info> getActiveUniforms() override;
        int getActiveUniformsCount() override;

    private:
    	unsigned int m_ProgramId = 0;
}; 
}


#endif
