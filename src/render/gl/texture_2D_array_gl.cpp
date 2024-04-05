
#include "texture_2D_array_gl.h"

using namespace modelViewer::render;

shader_texture_type texture_2D_array_gl::getType() const
{
	return shader_texture_type::texture2DArray;
}

texture_2D_array_gl::texture_2D_array_gl(texture_2D_array_gl_desc& desc)
{
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureId);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, desc.width, desc.height, desc.layers, 0, desc.format, desc.type, nullptr);


	if (desc.enableDepthCompare)
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

	glObjectLabel(GL_TEXTURE, m_TextureId, -1, desc.name.data());
	glBindTexture(GL_TEXTURE_2D_ARRAY,0);
}

void texture_2D_array_gl::BindAsFrameBuffer(int layer)
{
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureId, 0, layer);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}
