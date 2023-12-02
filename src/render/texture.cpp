#include "texture.h"
#include <utility>
#include <GL/glew.h>

using namespace modelViewer::render;

texture::texture(texture_setup texture_setup) : m_Asset{std::move(texture_setup.m_Asset)}, m_TextureFilteringMin{texture_setup.m_Texture_Filtering_Min}, m_TextureFilteringMig{texture_setup.m_Texture_Filtering_Mig}, m_TextureWrapping{texture_setup.m_Texture_Wrapping}, m_IsMipMapActive{texture_setup.m_Is_Mip_Map_Active}, m_MipMapMinLevel{texture_setup.m_Mip_Map_Min_Level}, m_MipMapMaxLevel{texture_setup.m_Mip_Map_Max_Level} {
    
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_5x5x5_OES, m_Asset->getWidth(), m_Asset->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_Asset->getContent());

    glObjectLabel(GL_TEXTURE, m_TextureId, -1, m_Asset->getName().data());

    glActiveTexture(GL_TEXTURE0);

    if (m_IsMipMapActive)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    setFilteringModeMag(m_TextureFilteringMig);
    setFilteringModeMin(m_TextureFilteringMin);
    setWrappingMode(m_TextureWrapping);


    glBindTexture(GL_TEXTURE_2D, 0);
}

texture::~texture() {
    glDeleteTextures(1, &m_TextureId);
}

void texture::bind() const {

}

texture_filtering_mode
texture::getFilteringModeMin() const {
    return m_TextureFilteringMin;
}

texture_filtering_mode
texture::getFilteringModeMag() const {
    return m_TextureFilteringMig;
}

texture_wrapping_mode
texture::getWrappingMode() const {
    return m_TextureWrapping;
}

void
texture::setFilteringMode(
        texture_filtering_mode textureFilteringMin, texture_filtering_mode textureFilteringMig){
    setFilteringModeMin(textureFilteringMin);
    setFilteringModeMag(textureFilteringMig);
}

void
texture::setFilteringModeMin(
        texture_filtering_mode textureFiltering){
    if (isTextureOutOfMemory())
    {
        throw std::invalid_argument("texture is out of memory");
    }

    m_TextureFilteringMig = textureFiltering;
    setFilteringMode(m_TextureFilteringMig, GL_TEXTURE_MIN_FILTER);
}

void
texture::setFilteringModeMag(
        texture_filtering_mode textureFiltering){
    if (isTextureOutOfMemory())
    {
        throw std::invalid_argument("texture is out of memory");
    }

    m_TextureFilteringMig = textureFiltering;
    setFilteringMode(m_TextureFilteringMig, GL_TEXTURE_MAG_FILTER);
}

void
texture::setFilteringMode(
        texture_filtering_mode textureFiltering,
        GLint filterType) {
    switch (textureFiltering) {

        case texture_filtering_mode::nearest:
            glTexParameteri(GL_TEXTURE_2D, filterType, GL_NEAREST);
            break;
        case texture_filtering_mode::linear:
            glTexParameteri(GL_TEXTURE_2D, filterType, GL_LINEAR);
            break;
        case texture_filtering_mode::linear_nearest:
            glTexParameteri(GL_TEXTURE_2D, filterType, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case texture_filtering_mode::nearest_linear:
            glTexParameteri(GL_TEXTURE_2D, filterType, GL_NEAREST_MIPMAP_LINEAR);
            break;
        case texture_filtering_mode::linear_linear:
            glTexParameteri(GL_TEXTURE_2D, filterType, GL_LINEAR_MIPMAP_LINEAR);
            break;
        case texture_filtering_mode::nearest_nearest:
            glTexParameteri(GL_TEXTURE_2D, filterType, GL_NEAREST_MIPMAP_NEAREST);
            break;
    }
}

void
texture::setWrappingMode(
        texture_wrapping_mode textureWrapping){
    m_TextureWrapping = textureWrapping;
    switch (textureWrapping) {

        case texture_wrapping_mode::clamp_to_edge:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case texture_wrapping_mode::mirror_clamp_to_edge:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
            break;
        case texture_wrapping_mode::clamp_to_border:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        case texture_wrapping_mode::wrap:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);
            break;
        case texture_wrapping_mode::repeat:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case texture_wrapping_mode::mirrored_repeat:
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

void
texture::setMipMapMinLevel(
        unsigned int minLevel) {
    if (m_MipMapMaxLevel < minLevel)
    {
        throw std::invalid_argument( "mipmap min level is bigger than mipmap max level");
    }

    m_MipMapMinLevel = minLevel;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, minLevel);
}

void
texture::setMipMapMaxLevel(
        unsigned int maxLevel) {
    if (maxLevel < m_MipMapMinLevel)
    {
        throw std::invalid_argument( "mipmap max level is smaller than mipmap min level");
    }

    m_MipMapMaxLevel = maxLevel;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, maxLevel);
}

bool
texture::isTextureOutOfMemory() {
    GLboolean state;
    return !glAreTexturesResident(1, &m_TextureId, &state);
}
