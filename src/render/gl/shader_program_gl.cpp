#include "shader_program_gl.h"
#include "glew/include/GL/glew.h"
#include "texture_gl.h"
#include <glm/gtc/type_ptr.hpp>

using namespace modelViewer::render;
using namespace modelViewer::res;

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
	reflectTextures();
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
	applyPipelineState();
	bindTextures();
}

shader_program_gl::~shader_program_gl() {
    glDeleteProgram(m_ProgramId);
}

int shader_program_gl::getAttributeLocation(const std::string& attributeName)
{
    return glGetAttribLocation(m_ProgramId, attributeName.c_str());
}

//TODO refactor this to query the uniforms at the ctor and use a map for fast check 
int shader_program_gl::getUniformLocation(const std::string& uniformName, bool optional) const {

	glUseProgram(m_ProgramId);
    int loc = glGetUniformLocation(m_ProgramId, uniformName.c_str());
	if(loc == -1 && !optional)
	{
		throw std::runtime_error("uniform: " + uniformName + " not found");
	}
	return loc;
}

void shader_program_gl::setUniform(const std::string& name, glm::vec3& vec3, bool optional) {
	const int loc = getUniformLocation(name, optional);
	if (loc > -1) {
		glUniform3f(loc, vec3.x,vec3.y,vec3.z);
	}
}

void shader_program_gl::setUniform(const std::string& name, glm::vec4& vec4, bool optional) {
	const int loc = getUniformLocation(name, optional);
	if (loc > -1) {
		glUniform4f(loc, vec4.x,vec4.y,vec4.z,vec4.w);
	}
}

void shader_program_gl::setUniform(const std::string& name, float value, bool optional) {
	const int loc = getUniformLocation(name, optional);
	if (loc > -1) {
		glUniform1f(loc, value);
	}
}

void shader_program_gl::setUniform(const std::string& name, int value, bool optional) {
	const int loc = getUniformLocation(name, optional);
	if (loc > -1) {
		glUniform1i(loc, value);
	}
}

void shader_program_gl::setUniform(const std::string& name, glm::mat4& mat, bool optional) {
	const int loc = getUniformLocation(name, optional);
	if (loc > -1) {
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(mat));
	}
}

void shader_program_gl::setUniform(const std::string& name, bool value, bool optional)
{
	const int loc = getUniformLocation(name, optional);
	if (loc > -1) {
		glUniform1i(loc, value);
	}
}


shader_texture_type getUniformType(GLenum internalType) {
    switch (internalType) {
		case GL_SAMPLER_1D:
			return shader_texture_type::texture1D;
        case GL_SAMPLER_2D:
        case GL_SAMPLER_2D_SHADOW:
            return shader_texture_type::texture2D;
		case GL_SAMPLER_3D:
			return shader_texture_type::texture3D;
		case GL_SAMPLER_1D_ARRAY:
			return shader_texture_type::texture1DArray;
		case GL_SAMPLER_2D_ARRAY:
			return shader_texture_type::texture2DArray;
        case GL_SAMPLER_CUBE:
            return shader_texture_type::textureCube;
        default:
            throw std::runtime_error("unsupported internal type");
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

void shader_program_gl::reflectTextures() {
	
    const int uniformsCount = getActiveUniformsCount();
    const int bufSize = 64;
    GLchar nameBuffer[bufSize];
    GLsizei length;
    GLint size;
    GLenum type;

    for (int i = 0; i < uniformsCount; ++i) {
        glGetActiveUniform(m_ProgramId, i, bufSize, &length, &size, &type, nameBuffer);
		auto name = std::string(nameBuffer);
        m_TextureSlots.push_back({name,getUniformType(type)});
		m_BoundTextures.push_back(nullptr);
    }
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

GLenum getDepthCompareFunction(depth_buffer_compare compare)
{
	switch (compare) {
		case depth_buffer_compare::never:
			return GL_NEVER;
		case depth_buffer_compare::always:
			return GL_ALWAYS;
		case depth_buffer_compare::lessOrEqual:
			return GL_LEQUAL;
		case depth_buffer_compare::less:
			return GL_LESS;
		case depth_buffer_compare::greaterOrEqual:
			return GL_GEQUAL;
		case depth_buffer_compare::greater:
			return GL_GREATER;
		case depth_buffer_compare::equal:
			return GL_EQUAL;
		case depth_buffer_compare::notEqual:
			return GL_NOTEQUAL;
		default:
			throw std::runtime_error("not supported!");
	}
}

void shader_program_gl::applyPipelineState() const {
	switch (m_FaceMode) {
		case cull_face_mode::front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			
			break;
		case cull_face_mode::back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case cull_face_mode::disabled:
			glDisable(GL_CULL_FACE);
			break;
		default:
			throw std::runtime_error("not imp");
	}

	if (m_DepthOptions.enabled)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(m_DepthOptions.writeEnabled);
		glDepthFunc(getDepthCompareFunction(m_DepthOptions.compare));
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	

	if (m_AlphaBlending)
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void shader_program_gl::setCullFaceMode(cull_face_mode mode) {
	m_FaceMode = mode;
}

bool shader_program_gl::hasUniform(const std::string& name) const
{
	return getUniformLocation(name, true) > -1;
}

void shader_program_gl::setDepthMap(depth_buffer_options& options)
{
	m_DepthOptions = options;
}

const std::vector<shader_texture_slot>& shader_program_gl::getTextureSlots()
{
	return m_TextureSlots;
}

void shader_program_gl::bindTexture(int slotIndex, std::shared_ptr<render::texture>& texture)
{
	if (m_TextureSlots.size() >= slotIndex)
	{
		throw std::runtime_error("texture slot is out of bounds");
	}
	
	auto texture_gl = std::dynamic_pointer_cast<render::texture_gl>(texture);
	if (texture_gl == nullptr)
	{
		throw std::runtime_error("texture slot is not a opengl texture");
	}
	
	m_BoundTextures[slotIndex] = texture_gl;
}

void shader_program_gl::bindTextures()
{
	for (int i = 0; i < m_BoundTextures.size(); ++i)
	{
		auto texture = m_BoundTextures[i];
		if (texture == nullptr)
		{
			throw std::runtime_error("texture slot is not bound");
		}
		texture->active(i);
	}
}

int shader_program_gl::getTextureSlot(const std::string& textureName)
{
	int slotIndex = 0;
	for (auto& slot : m_TextureSlots) {
		if(slot.name == textureName)
		{
			return slotIndex;
		}
		slotIndex++;
	}
	return -1;
}

bool shader_program_gl::bindTexture(const std::string& name, std::shared_ptr<render::texture>& texture)
{
	auto slot = getTextureSlot(name);
	if (slot >= 0)
	{
		bindTexture(slot, texture);
		return true;
	}
	return false;
}

void shader_program_gl::setAlphaBlending(bool enabled)
{
	m_AlphaBlending = enabled;
}

topology_mode shader_program_gl::getTopology()
{
	return m_Topology;
}

void shader_program_gl::setTopology(topology_mode topology)
{
	//TODO maybe use it somewhere 
	m_Topology = topology;
}


