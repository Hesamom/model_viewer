
#include <GL/glew.h>
#include "framebuffer.h"

modelViewer::render::framebuffer::framebuffer()
{
	glGenFramebuffers(1, &m_Id);
}

void modelViewer::render::framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
	if (m_DepthTextureId > 0)
	{
		//bind depth texture 
	}
}

void modelViewer::render::framebuffer::attachDepth(int width, int height)
{
	glGenTextures(1, &m_DepthTextureId);
	glBindTexture(GL_TEXTURE_2D, m_DepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//TODO maybe change filtering later?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void modelViewer::render::framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

modelViewer::render::framebuffer::~framebuffer()
{
	glDeleteFramebuffers(1, &m_Id);
	if (m_DepthTextureId > -1)
	{
		glDeleteTextures(1, &m_DepthTextureId);
	}
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
