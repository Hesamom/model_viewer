#include "texture.h"
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

texture::texture(texture_setup& texture_setup)
{
    m_Setup = texture_setup;
    auto optimalFormat = texture_format::getOptimalFormat(m_Setup.m_Asset->getChannelType());

    glGenTextures(1, &m_TextureId);
    setBind(true);
    
    glTexImage2D(GL_TEXTURE_2D, 0, optimalFormat, m_Setup.m_Asset->getWidth(), m_Setup.m_Asset->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_Setup.m_Asset->getContent());

    glObjectLabel(GL_TEXTURE, m_TextureId, -1, m_Setup.m_Asset->getName().data());

    if (m_Setup.m_Is_Mip_Map_Active)
    {
        //TODO have to check if specifying the level before generation affects generation or not 
        glGenerateMipmap(GL_TEXTURE_2D);
        setMipMapLevels(m_Setup.m_Mip_Map_Min_Level, m_Setup.m_Mip_Map_Max_Level);
    }

    setFilteringModeMag(m_Setup.m_Texture_Filtering_Mag);
    setFilteringModeMin(m_Setup.m_Texture_Filtering_Min);
    setWrappingMode(m_Setup.m_Texture_Wrapping);

    setBind(false);
}

texture::~texture() {
    glDeleteTextures(1, &m_TextureId);
}

void texture::active(int index) {
    int firstIndex = GL_TEXTURE0;
    int slotIndex = firstIndex + index;
    glActiveTexture(slotIndex);
    setBind(true);
}

texture_filtering_mode
texture::getFilteringModeMin() const {
    return m_Setup.m_Texture_Filtering_Min;
}

texture_filtering_mode
texture::getFilteringModeMag() const {
    return m_Setup.m_Texture_Filtering_Mag;
}

texture_wrapping_mode
texture::getWrappingMode() const {
    return m_Setup.m_Texture_Wrapping;
}

void
texture::setFilteringMode(
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
texture::setFilteringModeMin(
        texture_filtering_mode textureFiltering){
    if (!isTextureResident())
    {
        throw std::runtime_error("texture is not resident");
    }

    m_Setup.m_Texture_Filtering_Min = textureFiltering;
    setBind(true);
    setFilteringModeInternal(textureFiltering, GL_TEXTURE_MIN_FILTER);
}

void
texture::setFilteringModeMag(
        texture_filtering_mode textureFiltering){
    if (!isTextureResident())
    {
        throw std::runtime_error("texture is not resident");
    }

    m_Setup.m_Texture_Filtering_Mag = textureFiltering;
    setBind(true);
    setFilteringModeInternal(textureFiltering, GL_TEXTURE_MAG_FILTER);
}

void
texture::setWrappingMode(
        texture_wrapping_mode textureWrapping){
    m_Setup.m_Texture_Wrapping = textureWrapping;
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
texture::getMipMapMinLevel() {
    return m_Setup.m_Mip_Map_Min_Level;
}

unsigned int
texture::getMipMapMaxLevel() {
    return  m_Setup.m_Mip_Map_Max_Level;
}


bool
texture::isTextureResident() {
    GLboolean state;
    //return !glAreTexturesResident(1, &m_TextureId, &state);
    return true;
}

void texture::setMipMapLevels(unsigned int min, unsigned int max) {
    if (max <= min)
    {
        throw std::invalid_argument( "mipmap min level is bigger than mipmap max level");
    }
    if (!m_Setup.m_Is_Mip_Map_Active)
    {
        throw std::runtime_error( "mipmap levels can not be set when mip map is not active!");
    }

    if(m_Setup.m_Mip_Map_Min_Level != min)
    {
        setBind(true);
        m_Setup.m_Mip_Map_Min_Level = min;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, min);
    }
    if ( m_Setup.m_Mip_Map_Max_Level != max)
    {
        setBind(true);
        m_Setup.m_Mip_Map_Max_Level = max;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max);
    }
}

void texture::setBind(bool bind) {

    static unsigned int m_BoundTexture = -1;
    if(bind && m_BoundTexture == m_TextureId)
    {
        return;
    }
    
    if (bind)
    {
        m_BoundTexture = m_TextureId;
    }
    else
    {
        m_BoundTexture = 0;
    }

    glBindTexture(GL_TEXTURE_2D, m_BoundTexture);
}

modelViewer::res::texture_asset_type texture::getType() const {
    return m_Setup.type;
}
