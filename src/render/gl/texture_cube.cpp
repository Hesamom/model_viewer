#include "texture_cube.h"
#include "GL/glew.h"
#include "../texture_format.h"

modelViewer::render::texture_cube::texture_cube(texture_setup& textureSetup) {
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);

    auto optimalFormat = texture_format::getOptimalFormat(textureSetup.assets[0]->getChannelType(), textureSetup.compress);

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
