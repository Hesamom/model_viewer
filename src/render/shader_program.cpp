#include "shader_program.h"
#include "glew/include/GL/glew.h"
#include <glm/gtc/type_ptr.hpp>

using namespace modelViewer::render;


//TODO find a way to remove ctors duplications 
shader_program::shader_program(std::vector<shader> &shaders) {
    
    m_ProgramId = glCreateProgram();

    for(auto& shader : shaders)
    {
        glAttachShader(m_ProgramId, shader.getId());
        shader.destroy();
    }

    glLinkProgram(m_ProgramId);
}

shader_program::shader_program(std::initializer_list<shader> shaders) {
    
    m_ProgramId = glCreateProgram();
    
    for(auto& shader : shaders)
    {
        glAttachShader(m_ProgramId, shader.getId());
        shader.destroy();
    }
    
    glLinkProgram(m_ProgramId);
}


std::string shader_program::getLinkLog() {
    if(m_ProgramId == 0)
    {
        return {};
    }

    int length;
    glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &length);
    std::string message(length, ' ');
    glGetProgramInfoLog(m_ProgramId, length, &length, message.data());
    return message;
}

bool shader_program::isLinked() {
    int result;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &result);
    return result == GL_TRUE;
}

void shader_program::bind() {
	
	//TODO add guards 
    glUseProgram(m_ProgramId);
}

shader_program::~shader_program() {
    glDeleteProgram(m_ProgramId);
}

int shader_program::getAttributeLocation(const std::string& attributeName)
{
    return glGetAttribLocation(m_ProgramId, attributeName.c_str());
}

int shader_program::getUniformLocation(const std::string& uniformName) {
    return glGetUniformLocation(m_ProgramId, uniformName.c_str());
}

void shader_program::setUniformVector3(int location, glm::vec3 vec3) {
     glUniform3f(location, vec3.x,vec3.y,vec3.z);
}

void shader_program::setUniformVector4(int location, glm::vec4 vec4) {
    bind();
    glUniform4f(location, vec4.x,vec4.y,vec4.z,vec4.w);
}

void shader_program::setUniformFloat(int location, float value) {
    glUniform1f(location, value);
}

void shader_program::setUniformInt(int location, int value) {
    glUniform1i(location, value);
}

void shader_program::setUniformMatrix4(int index, glm::mat4& mat) {
    glUniformMatrix4fv(index, 1, false, glm::value_ptr(mat));
}

void shader_program::validate()
{
	glValidateProgram(m_ProgramId);
	int valid = 0;
	glGetProgramiv(m_ProgramId, GL_VALIDATE_STATUS, &valid);
	if (valid == GL_FALSE)
	{
		throw std::runtime_error("validation failed");
	}
	
	auto log = getLinkLog();
	if(!log.empty())
	{
		std::cout << log << std::endl;
	}
}


