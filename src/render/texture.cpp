#include "texture.h"
#include <utility>
#include <GL/glew.h>

using namespace modelViewer::render;

texture::texture(std::shared_ptr<modelViewer::res::texture_asset> asset) : m_Asset{std::move(asset)} {
    
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Asset->getWidth(), m_Asset->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_Asset->getContent());

    glObjectLabel(GL_TEXTURE, m_TextureId, -1, m_Asset->getName().data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

texture::~texture() {
    glDeleteTextures(1, &m_TextureId);
}

void texture::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
}
