
#include "shader.h"
#include <utility>
#include "glew/include/GL/glew.h"

using namespace modelViewer::res;

shaderType modelViewer::render::shader::getType() {
    return m_Asset->getType();
}

modelViewer::render::shader::shader(std::shared_ptr<modelViewer::res::shader_asset>& asset): m_Asset{asset} {
    
   
}

unsigned int getShaderType(shaderType type)
{
    switch (type) {
        case shaderType::none:
            throw std::runtime_error("...");
        case shaderType::fragment:
            return GL_FRAGMENT_SHADER;
        case shaderType::vertex:
            return GL_VERTEX_SHADER;
        case shaderType::tesselation_control:
            return GL_TESS_CONTROL_SHADER;
        case shaderType::tesselation_evaluation:
            return GL_TESS_EVALUATION_SHADER;
        case shaderType::geometry:
            return GL_GEOMETRY_SHADER;
        case shaderType::compute:
            return GL_COMPUTE_SHADER;
    }
}

void modelViewer::render::shader::compile() {
    
    m_ShaderId = glCreateShader(getShaderType(m_Asset->getType()));
    auto sourceString = m_Asset->getSource();
    auto source = sourceString.c_str();
    glShaderSource(m_ShaderId, 1, &source, nullptr);
    glCompileShader(m_ShaderId);
}

std::string modelViewer::render::shader::getCompilationLog() {
    
    if(m_ShaderId == 0)
    {
        return std::string();
    }
    
    int length;
    glGetShaderiv(m_ShaderId, GL_INFO_LOG_LENGTH, &length);
    std::string message = "compilation of shader in path: " + m_Asset->getPath() + " failed due to following error(s)"
                                                                                   ": \n";
    std::string buffer(length, ' ');
    glGetShaderInfoLog(m_ShaderId, length, &length, buffer.data());
    message.append(buffer);
    return message;
}

bool modelViewer::render::shader::isCompiled() {
    int result;
    glGetShaderiv(m_ShaderId, GL_COMPILE_STATUS, &result);
    return result == GL_TRUE;
}

modelViewer::render::shader::~shader() {
    glDeleteShader(m_ShaderId);
}

unsigned int modelViewer::render::shader::getId() {
    return m_ShaderId;
}
