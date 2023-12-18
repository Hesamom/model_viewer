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

void shader_program::setUniform(int location, glm::vec3 vec3) {
     glUniform3f(location, vec3.x,vec3.y,vec3.z);
}



void shader_program::setUniform(int location, glm::vec4 vec4) {
    bind();
    glUniform4f(location, vec4.x,vec4.y,vec4.z,vec4.w);
}

void shader_program::setUniform(int location, float value) {
    glUniform1f(location, value);
}

void shader_program::setUniform(int location, int value) {
    glUniform1i(location, value);
}

void shader_program::setUniform(int index, glm::mat4& mat) {
    glUniformMatrix4fv(index, 1, false, glm::value_ptr(mat));
}

shader_uniform_type getUniformType(GLenum internalType) {
    switch (internalType) {
        case GL_SAMPLER_2D:
            return shader_uniform_type::sampler2D;
        case GL_SAMPLER_2D_SHADOW:
            return shader_uniform_type::sampler2DShadow;
        case GL_SAMPLER_CUBE:
            return shader_uniform_type::samplerCube;
        default:
            return shader_uniform_type::none;
    }
}

std::vector<shader_uniform_info> shader_program::getActiveUniforms() {
    

    const int uniformsCount = getActiveUniformsCount();
    const int bufSize = 64;
    GLchar nameBuffer[bufSize];
    GLsizei length;
    GLint size;
    GLenum type;

    std::vector<shader_uniform_info> uniforms;
    for (int i = 0; i < uniformsCount; ++i) {
        glGetActiveUniform(m_ProgramId, i, bufSize, &length, &size, &type, nameBuffer);
        uniforms.push_back({std::string(nameBuffer),getUniformType(type) });
    }

    return uniforms;
}

int shader_program::getActiveUniformsCount() {
    int count = 0;
    glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORMS, &count);
    return count;
}

void shader_program::validateLinking()
{
	if (!isLinked())
	{
        std::cerr<< getLinkLog() << std::endl;
		throw std::runtime_error("linking failed");
	}
	
	auto log = getLinkLog();
	if(!log.empty())
	{
		std::cout << log << std::endl;
	}
}


