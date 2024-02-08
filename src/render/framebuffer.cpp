
#include <GL/glew.h>
#include "framebuffer.h"

//TODO use Texture_2D and Texture_Array classes instead of manual creation, need new Texture_Array and new ctor for Texture_2D

modelViewer::render::framebuffer::framebuffer()
{
	glGenFramebuffers(1, &m_Id);
}

void modelViewer::render::framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
}

unsigned int modelViewer::render::framebuffer::createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare)
{
	glGenTextures(1, &m_ArrayDepthTextureId);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ArrayDepthTextureId);
	
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, width, height, layers, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	
	
	if (enableDepthCompare)
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE , GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC  , GL_GREATER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	return m_ArrayDepthTextureId;
}


unsigned int modelViewer::render::framebuffer::createDepthTexture(int width, int height, bool enableDepthCompare)
{
	glGenTextures(1, &m_DepthTextureId);
	glBindTexture(GL_TEXTURE_2D, m_DepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	
	

	if(enableDepthCompare)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE , GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC  , GL_GREATER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	

	glBindTexture(GL_TEXTURE_2D,0);
	return m_DepthTextureId;
}

void modelViewer::render::framebuffer::attachDepthTexture() {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void modelViewer::render::framebuffer::attachDepthTextureArray(int layer) {
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ArrayDepthTextureId, 0, layer);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void modelViewer::render::framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

modelViewer::render::framebuffer::~framebuffer()
{
	//TODO unbind them first 
	if (m_DepthTextureId > -1)
	{
		glDeleteTextures(1, &m_DepthTextureId);
	}
	if (m_ArrayDepthTextureId > -1)
	{
		glDeleteTextures(1, &m_ArrayDepthTextureId);
	}

	unbind();
	glDeleteFramebuffers(1, &m_Id);
}

void modelViewer::render::framebuffer::activateDepthMap(int slot)
{
	if (m_DepthTextureId < 0)
	{
		throw std::runtime_error("no depth map to activate!");
	}
	
	int firstIndex = GL_TEXTURE0;
	int slotIndex = firstIndex + slot;
	glActiveTexture(slotIndex);
	glBindTexture(GL_TEXTURE_2D, m_DepthTextureId);
}

void modelViewer::render::framebuffer::activateDepthMapArray(int slot)
{
	if (m_ArrayDepthTextureId < 0)
	{
		throw std::runtime_error("no depth map to activate!");
	}
	
	int firstIndex = GL_TEXTURE0;
	int slotIndex = firstIndex + slot;
	glActiveTexture(slotIndex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ArrayDepthTextureId);
}

