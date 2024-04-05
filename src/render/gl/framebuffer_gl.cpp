#include <GL/glew.h>
#include "framebuffer_gl.h"

using namespace modelViewer::render;

framebuffer_gl::framebuffer_gl(std::string& name)
{
	m_Name = name; 
	glGenFramebuffers(1, &m_BufferId);
}

void framebuffer_gl::createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare)
{
	texture_2D_array_gl_desc desc;
	desc.width = width;
	desc.height = height;
	desc.layers = layers;
	desc.format = GL_DEPTH_COMPONENT;
	desc.type = GL_UNSIGNED_BYTE;
	desc.enableDepthCompare = enableDepthCompare;
	desc.name = m_Name + "_depth_array";
	m_DepthArrayTexture = std::make_shared<texture_2D_array_gl>(desc);
}

void framebuffer_gl::createDepthTexture(int width, int height, bool enableDepthCompare)
{
	auto textureName = m_Name + "_depth_texture";
	m_DepthTexture = std::make_shared<texture_2D_gl>(width, height, GL_DEPTH_COMPONENT, enableDepthCompare, textureName);
}

void framebuffer_gl::createCubeMap(int size)
{
	if (size < 1)
	{
		throw std::length_error("size is smaller than 1");
	}

	auto textureName = m_Name + "_cube_texture";
	m_ColorCubeTexture = std::make_shared<texture_cube_gl>(size,textureName);
}

void framebuffer_gl::attachDepthTexture()
{
	if (m_DepthTexture)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferId);
		m_DepthTexture->BindAsDepthBuffer();
	}
}

void framebuffer_gl::attachCubeMapFace(int index)
{
	if (m_ColorCubeTexture)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferId);
		m_ColorCubeTexture->BindAsFrameBuffer(index);
	}
}

void framebuffer_gl::attachDepthTextureArray(int layer)
{
	if (m_DepthArrayTexture)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferId);
		m_DepthArrayTexture->BindAsFrameBuffer(layer);
	}
}

void framebuffer_gl::clearColorBuffer(const glm::vec4& color) 
{
	glClearBufferfv(GL_COLOR, 0, &color.x);
}

void framebuffer_gl::clearDepthBuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void framebuffer_gl::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

framebuffer_gl::~framebuffer_gl()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_BufferId);
}

std::shared_ptr<texture> framebuffer_gl::getDepthMap()
{
	return m_DepthTexture;
}

std::shared_ptr<texture> framebuffer_gl::getDepthMapArray()
{
	return m_DepthArrayTexture;
}

std::shared_ptr<texture> framebuffer_gl::getColorCubeMap()
{
	return m_ColorCubeTexture;
}

void framebuffer_gl::createColorBuffer(int width, int height)
{
	throw std::runtime_error("Not implemented");
}



