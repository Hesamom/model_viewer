#include "texture_cube_gl.h"
#include "GL/glew.h"
#include "texture_format.h"

modelViewer::render::texture_cube_gl::texture_cube_gl(texture_setup& textureSetup) {
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);

    auto optimalFormat = texture_format::getOptimalFormat(textureSetup.assets[0]->getChannelType(), false);

	assert(textureSetup.assets.size() == 6);
	
    for (int i = 0;i < 6; ++i)
	{
        auto texture = textureSetup.assets[i];
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, optimalFormat, texture->getWidth(), texture->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->getContent()
        );
    }

	
	//TODO filtering and clamping needs to be taken from setup
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

modelViewer::render::shader_texture_type modelViewer::render::texture_cube_gl::getType() const
{
	return modelViewer::render::shader_texture_type::textureCube;
}

modelViewer::render::texture_cube_gl::texture_cube_gl(int size, std::string& name)
{
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size,
			size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	glObjectLabel(GL_TEXTURE, m_TextureId, -1, name.c_str());
}

void modelViewer::render::texture_cube_gl::BindAsFrameBuffer(int faceIndex)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + faceIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, m_TextureId, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + faceIndex);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + faceIndex);
}
