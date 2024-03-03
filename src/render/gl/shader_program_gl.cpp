﻿#include "shader_program_gl.h"
#include "glew/include/GL/glew.h"
#include <glm/gtc/type_ptr.hpp>

using namespace modelViewer::render;


//TODO find a way to remove ctors duplications 
shader_program_gl::shader_program_gl(std::vector<shader_gl> &shaders) {
    
    m_ProgramId = glCreateProgram();

    for(auto& shader : shaders)
    {
        glAttachShader(m_ProgramId, shader.getId());
        shader.destroy();
    }

    glLinkProgram(m_ProgramId);
}

shader_program_gl::shader_program_gl(std::initializer_list<shader_gl> shaders) {
    
    m_ProgramId = glCreateProgram();
    
    for(auto& shader : shaders)
    {
        glAttachShader(m_ProgramId, shader.getId());
        shader.destroy();
    }
    
    glLinkProgram(m_ProgramId);
}


std::string shader_program_gl::getLinkLog() {
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

bool shader_program_gl::isLinked() {
    int result;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &result);
    return result == GL_TRUE;
}

void shader_program_gl::bind() {
	
	//TODO add guards 
    glUseProgram(m_ProgramId);
}

shader_program_gl::~shader_program_gl() {
    glDeleteProgram(m_ProgramId);
}

int shader_program_gl::getAttributeLocation(const std::string& attributeName)
{
    return glGetAttribLocation(m_ProgramId, attributeName.c_str());
}

int shader_program_gl::getUniformLocation(const std::string& uniformName) {
    return glGetUniformLocation(m_ProgramId, uniformName.c_str());
}

void shader_program_gl::setUniform(int location, glm::vec3 vec3) {
     glUniform3f(location, vec3.x,vec3.y,vec3.z);
}



void shader_program_gl::setUniform(int location, glm::vec4 vec4) {
    bind();
    glUniform4f(location, vec4.x,vec4.y,vec4.z,vec4.w);
}

void shader_program_gl::setUniform(int location, float value) {
    glUniform1f(location, value);
}

void shader_program_gl::setUniform(int location, int value) {
    glUniform1i(location, value);
}

void shader_program_gl::setUniform(int index, glm::mat4& mat) {
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

shader_texture_usage getSamplerUsage(std::string& name) {
	
	if(name.starts_with("u_mat.diffuse"))
	{
		return shader_texture_usage::diffuse;
	}
	if(name.starts_with("u_mat.specular"))
	{
		return shader_texture_usage::specular;
	}
	if(name.starts_with("u_mat.normal"))
	{
		return shader_texture_usage::normal;
	}
	
	return shader_texture_usage::none;
}

std::vector<shader_uniform_info> shader_program_gl::getActiveUniforms() {
    

    const int uniformsCount = getActiveUniformsCount();
    const int bufSize = 64;
    GLchar nameBuffer[bufSize];
    GLsizei length;
    GLint size;
    GLenum type;

    std::vector<shader_uniform_info> uniforms;
    for (int i = 0; i < uniformsCount; ++i) {
        glGetActiveUniform(m_ProgramId, i, bufSize, &length, &size, &type, nameBuffer);
		auto name = std::string(nameBuffer);
        uniforms.push_back({name,getUniformType(type), getSamplerUsage(name)});
    }

    return uniforms;
}

int shader_program_gl::getActiveUniformsCount() {
    int count = 0;
    glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORMS, &count);
    return count;
}

void shader_program_gl::validateLinking()
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

