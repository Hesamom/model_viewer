#include "texture_2D.h"
#include "texture_format.h"
#include <GL/glew.h>

using namespace modelViewer::render;

GLint getFormat(int channelsCount)
{
    switch (channelsCount) {
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;

        default:
            throw std::runtime_error("not supported color channel");
    }
}

texture_2D::texture_2D(texture_setup& texture_setup)
{
    m_Setup = texture_setup;
	assert(!m_Setup.assets.empty());
	
	auto textureAsset = m_Setup.assets[0];
    auto optimalFormat = texture_format::getOptimalFormat(textureAsset->getChannelType(), m_Setup.compress);

    glGenTextures(1, &m_TextureId);
    setBind(true);
    
    glTexImage2D(GL_TEXTURE_2D, 0, optimalFormat, textureAsset->getWidth(), m_Setup.assets[0]->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
		textureAsset->getContent());

    glObjectLabel(GL_TEXTURE, m_TextureId, -1, textureAsset->getName().data());

    if (m_Setup.isMipMapActive)
    {
        //TODO have to check if specifying the level before generation affects generation or not 
        glGenerateMipmap(GL_TEXTURE_2D);
        setMipMapLevels(m_Setup.mipMapMinLevel, m_Setup.mipMapMaxLevel);
    }

    setFilteringModeMag(m_Setup.filteringMag);
    setFilteringModeMin(m_Setup.filteringMin);
    setWrappingMode(m_Setup.wrapping);

    setBind(false);
}

texture_2D::~texture_2D() {
    glDeleteTextures(1, &m_TextureId);
}

texture_filtering_mode
texture_2D::getFilteringModeMin() const {
    return m_Setup.filteringMin;
}

texture_filtering_mode
texture_2D::getFilteringModeMag() const {
    return m_Setup.filteringMag;
}

texture_wrapping_mode
texture_2D::getWrappingMode() const {
    return m_Setup.wrapping;
}

void
texture_2D::setFilteringMode(
        texture_filtering_mode textureFilteringMin, texture_filtering_mode textureFilteringMag){
    setFilteringModeMin(textureFilteringMin);
    setFilteringModeMag(textureFilteringMag);
}

void setFilteringModeInternal(
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
texture_2D::setFilteringModeMin(
        texture_filtering_mode textureFiltering){
    if (!isTextureResident())
    {
        throw std::runtime_error("texture is not resident");
    }

    m_Setup.filteringMin = textureFiltering;
    setBind(true);
    setFilteringModeInternal(textureFiltering, GL_TEXTURE_MIN_FILTER);
}

void
texture_2D::setFilteringModeMag(
        texture_filtering_mode textureFiltering){
    if (!isTextureResident())
    {
        throw std::runtime_error("texture is not resident");
    }

    m_Setup.filteringMag = textureFiltering;
    setBind(true);
    setFilteringModeInternal(textureFiltering, GL_TEXTURE_MAG_FILTER);
}

void
texture_2D::setWrappingMode(
        texture_wrapping_mode textureWrapping){
    m_Setup.wrapping = textureWrapping;
    setBind(true);
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
texture_2D::getMipMapMinLevel() {
    return m_Setup.mipMapMinLevel;
}

unsigned int
texture_2D::getMipMapMaxLevel() {
    return  m_Setup.mipMapMaxLevel;
}


bool
texture_2D::isTextureResident() {
    GLboolean state;
    //return !glAreTexturesResident(1, &m_TextureId, &state);
    return true;
}

void texture_2D::setMipMapLevels(unsigned int min, unsigned int max) {
    if (max <= min)
    {
        throw std::invalid_argument( "mipmap min level is bigger than mipmap max level");
    }
    if (!m_Setup.isMipMapActive)
    {
        throw std::runtime_error( "mipmap levels can not be set when mip map is not active!");
    }

    if(m_Setup.mipMapMinLevel != min)
    {
        setBind(true);
        m_Setup.mipMapMinLevel = min;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, min);
    }
    if (m_Setup.mipMapMaxLevel != max)
    {
        setBind(true);
        m_Setup.mipMapMaxLevel = max;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max);
    }
}
