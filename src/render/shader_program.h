#ifndef MODEL_VIEWER_SHADER_PROGRAM_H
#define MODEL_VIEWER_SHADER_PROGRAM_H

#include "../resource/shader_loader.h"
#include "shader.h"
#include "glm/glm.hpp"

namespace modelViewer::render
{
    enum class shader_uniform_type {
        none,
        sampler2D,
        sampler2DShadow,
        samplerCube
    };
    
    struct shader_uniform_info {
        std::string name;
        shader_uniform_type type = shader_uniform_type::none;
        bool isSampler() const {
            return type == shader_uniform_type::sampler2D || type == shader_uniform_type::sampler2DShadow || type == shader_uniform_type::samplerCube;
        }

        bool isShadowSampler() const {
            return type == shader_uniform_type::sampler2DShadow;
        }
    };
    
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
        std::vector<shader_uniform_info> getActiveUniforms();
        int getActiveUniformsCount();
}; 
}


#endif
