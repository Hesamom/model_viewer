#include "texture.h"
#include <utility>
#include <GL/glew.h>

using namespace modelViewer::render;

texture::texture(std::shared_ptr<modelViewer::res::texture_asset> asset, textureFiltering textureFiltering,textureWrapping textureWrapping) : m_Asset{std::move(asset)}, m_TextureFiltering{textureFiltering}, m_TextureWrapping{textureWrapping} {
    
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    setFilteringMode(m_TextureFiltering);
    setWrappingMode(m_TextureWrapping);

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

textureFiltering
texture::getFilteringMode() const {
    return m_TextureFiltering;
}

textureWrapping
texture::getWrappingMode() const {
    return m_TextureWrapping;
}

void
texture::setFilteringMode(
        textureFiltering textureFiltering){
    m_TextureFiltering = textureFiltering;
    switch (textureFiltering) {

        case textureFiltering::nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case textureFiltering::linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case textureFiltering::nearest_Mipmap_Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            break;
        case textureFiltering::linear_Mipmap_Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case textureFiltering::nearest_Mipmap_Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            break;
        case textureFiltering::linear_Mipmap_Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
    }
}

void
texture::setWrappingMode(
        textureWrapping textureWrapping){
    m_TextureWrapping = textureWrapping;
    switch (textureWrapping) {

        case textureWrapping::clamp_To_Edge:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case textureWrapping::mirror_Clamp_ToEdge:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
            break;
        case textureWrapping::clamp_To_Border:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        case textureWrapping::wrap:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);
            break;
        case textureWrapping::repeat:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case textureWrapping::mirrored_Repeat:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
    }
}
