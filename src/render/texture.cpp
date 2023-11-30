#include "texture.h"
#include <utility>
#include <GL/glew.h>

using namespace modelViewer::render;

texture::texture(std::shared_ptr<modelViewer::res::texture_asset> asset, textureFiltering textureFilteringMin, textureFiltering textureFilteringMig, textureWrapping textureWrapping, bool isMipMapActive, unsigned int mipMapMinLevel ,unsigned int mipMapMaxLevel) : m_Asset{std::move(asset)}, m_TextureFilteringMin{textureFilteringMin}, m_TextureFilteringMig{textureFilteringMig}, m_TextureWrapping{textureWrapping}, m_IsMipMapActive{isMipMapActive}, m_MipMapMinLevel{mipMapMinLevel}, m_MipMapMaxLevel{mipMapMaxLevel} {
    
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    setFilteringModeMig(m_TextureFilteringMig);
    setFilteringModeMin(m_TextureFilteringMin);
    setWrappingMode(m_TextureWrapping);

    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_5x5x5_OES, m_Asset->getWidth(), m_Asset->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_Asset->getContent());

    glObjectLabel(GL_TEXTURE, m_TextureId, -1, m_Asset->getName().data());

    //TODO should unbind in the end
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (m_IsMipMapActive)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

texture::~texture() {
    glDeleteTextures(1, &m_TextureId);
}

void texture::bind() const {
    //TODO seems like one this is redundant 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

textureFiltering
texture::getFilteringModeMin() const {
    return m_TextureFilteringMin;
}

textureFiltering
texture::getFilteringModeMig() const {
    return m_TextureFilteringMig;
}

textureWrapping
texture::getWrappingMode() const {
    return m_TextureWrapping;
}

void
texture::setFilteringMode(
        textureFiltering textureFilteringMin, textureFiltering textureFilteringMig){
    setFilteringModeMin(textureFilteringMin);
    setFilteringModeMig(textureFilteringMig);
}

//TODO min and mag are almost duplicates, extract a function to call in both of them instead
void
texture::setFilteringModeMin(
        textureFiltering textureFiltering){
    //TODO ensure the texture is bound 
    m_TextureFilteringMig = textureFiltering;
    switch (textureFiltering) {

        case textureFiltering::nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
        case textureFiltering::linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
        case textureFiltering::nearest_Mipmap_Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            break;
        case textureFiltering::linear_Mipmap_Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case textureFiltering::nearest_Mipmap_Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            break;
        case textureFiltering::linear_Mipmap_Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
    }
}

void
texture::setFilteringModeMig(
        textureFiltering textureFiltering){
    m_TextureFilteringMig = textureFiltering;
    switch (textureFiltering) {

        case textureFiltering::nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case textureFiltering::linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case textureFiltering::nearest_Mipmap_Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            break;
        case textureFiltering::linear_Mipmap_Nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case textureFiltering::nearest_Mipmap_Linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            break;
        case textureFiltering::linear_Mipmap_Linear:
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

unsigned int
texture::getMipMapMinLevel() {
    return m_MipMapMinLevel;
}

unsigned int
texture::getMipMapMaxLevel() {
    return m_MipMapMaxLevel;
}

//TODO forgot to set fields 
//TODO add some range checks 
//TODO combine setting min and max together to make sure their values make sense
void
texture::setMipMapMinLevel(
        unsigned int minLevel) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, minLevel);
}

//TODO forgot to set fields 
void
texture::setMipMapMaxLevel(
        unsigned int maxLevel) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, maxLevel);
}
